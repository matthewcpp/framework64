#include "framework64/n64/renderer.h"

#include "framework64/matrix.h"

#include "framework64/n64/font.h"
#include "framework64/n64/image.h"
#include "framework64/n64/mesh.h"
#include "framework64/n64/texture.h"

#include <nusys.h>

#include <malloc.h>
#include <string.h>

/** based on G_RM_[AA]_[ZB]_TEX_EDGE macro definition. */
#define FW64_RENDER_MODE_3D_TEXTURED(renderer, cycle) \
	((renderer)->rendermode_aa_flags) | ((renderer)->rendermode_depth_test_flags) | IM_RD | CVG_DST_CLAMP |		\
	CVG_X_ALPHA | ALPHA_CVG_SEL | ZMODE_OPA | TEX_EDGE |	\
	GBL_c##cycle(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

#define FW64_RM_3D_TEXTURED(renderer) FW64_RENDER_MODE_3D_TEXTURED(renderer, 1)
#define FW64_RM_3D_TEXTURED2(renderer) FW64_RENDER_MODE_3D_TEXTURED(renderer, 2)

/** based on G_RM_[AA]_[ZB]_OPA_SURF macro definition. */
#define FW64_RENDER_MODE_3D_OPAQUE_SHADED(renderer, cycle) \
	((renderer)->rendermode_aa_flags) | ((renderer)->rendermode_depth_test_flags) | IM_RD | CVG_DST_CLAMP |		\
	ZMODE_OPA | ALPHA_CVG_SEL |				\
	GBL_c##cycle(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)

#define FW64_RM_3D_OPAQUE_SHADED(renderer) FW64_RENDER_MODE_3D_OPAQUE_SHADED(renderer, 1)
#define FW64_RM_3D_OPAQUE_SHADED2(renderer) FW64_RENDER_MODE_3D_OPAQUE_SHADED(renderer, 2)

static void fw64_n64_renderer_update_lighting_data(fw64Renderer* renderer);
static void fw64_n64_renderer_set_lighting_data(fw64Renderer* renderer);
static void fw64_n64_renderer_load_texture(fw64Renderer* renderer, fw64Texture* texture, int frame);

void fw64_n64_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height) {
    renderer->screen_size.x = screen_width;
    renderer->screen_size.y = screen_height;

    renderer->display_list = NULL;
    renderer->render_mode = FW64_RENDERER_MODE_UNSET;
    renderer->shading_mode = FW64_SHADING_MODE_UNSET;
    renderer->flags = FW64_RENDERER_FLAG_NONE;

    renderer->fill_color = GPACK_RGBA5551(255, 255, 255, 1);
    renderer->clear_color = GPACK_RGBA5551(0, 0, 0, 1);

    renderer->rendermode_aa_flags = AA_EN;
    fw64_renderer_set_depth_testing_enabled(renderer, 1);

    /*
      The viewport structure 
      The conversion from (-1,-1,-1)-(1,1,1).  The decimal part of 2-bit.
    */
    renderer->view_port.vp.vscale[0] = screen_width * 2;
    renderer->view_port.vp.vscale[1] = screen_height * 2;
    renderer->view_port.vp.vscale[2] = G_MAXZ / 2;
    renderer->view_port.vp.vscale[3] = 0;

    renderer->view_port.vp.vtrans[0] = screen_width * 2;
    renderer->view_port.vp.vtrans[1] = screen_height * 2;
    renderer->view_port.vp.vtrans[2] = G_MAXZ / 2;
    renderer->view_port.vp.vtrans[3] = 0;

    // set default lighting state - single white light
    Lights2 lights = gdSPDefLights2(
        25, 25, 25,
        255, 255, 255, 40, 40,40,
        255, 255, 255, -40, -40, 40
    );
    renderer->lights = lights;
    renderer->active_light_mask = 1;
}

void fw64_renderer_set_depth_testing_enabled(fw64Renderer* renderer, int enabled) {
    if (enabled)
        renderer->rendermode_depth_test_flags = Z_CMP | Z_UPD;
    else
        renderer->rendermode_depth_test_flags = 0;

    if (renderer->render_mode == FW64_RENDERER_MODE_UNSET)
        return;


    switch(renderer->shading_mode) {
        case FW64_SHADING_MODE_UNLIT_TEXTURED:
        case FW64_SHADING_MODE_GOURAUD_TEXTURED:
            gDPSetRenderMode(renderer->display_list++, FW64_RM_3D_TEXTURED(renderer), FW64_RM_3D_TEXTURED2(renderer));
            break;

        case FW64_SHADING_MODE_GOURAUD:
        case FW64_SHADING_MODE_UNLIT_VERTEX_COLORS:
            gDPSetRenderMode(renderer->display_list++, FW64_RM_3D_OPAQUE_SHADED(renderer), FW64_RM_3D_OPAQUE_SHADED2(renderer));
            break;

        // node FW64_SHADING_MODE_SPRITE does not currently utilize the z buffer
        default:
            break;
    }
}

int fw64_renderer_get_depth_testing_enabled(fw64Renderer* renderer) {
    return renderer->rendermode_depth_test_flags != 0;
}

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->clear_color = GPACK_RGBA5551(r, g, b, 1);
}

Gfx _rdp_init_static_dl[] = {
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetTextureDetail(G_TD_CLAMP),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetTextureConvert(G_TC_FILT),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetCombineKey(G_CK_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
    gsDPSetColorDither(G_CD_DISABLE),
    gsSPEndDisplayList()
};

void fw64_renderer_init_rdp(fw64Renderer* renderer) {
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE);
    gDPSetScissor(renderer->display_list++,G_SC_NON_INTERLACE, 0, 0, renderer->screen_size.x, renderer->screen_size.y);
    gSPDisplayList(renderer->display_list++, _rdp_init_static_dl);
    gDPSetDepthImage(renderer->display_list++,nuGfxZBuffer);
    gDPPipeSync(renderer->display_list++);
}


void fw64_renderer_init_rsp(fw64Renderer* renderer) {
    gSPViewport(renderer->display_list++, &renderer->view_port);
    gSPClearGeometryMode(renderer->display_list++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF);
    gSPSetGeometryMode(renderer->display_list++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH);
}

void fw64_renderer_clear_z_buffer(fw64Renderer* renderer) {
    //gDPSetDepthImage(renderer->display_list++, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b,renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
    gDPPipeSync(renderer->display_list++);
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    
    gDPSetFillColor(renderer->display_list++,(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
    gDPFillRectangle(renderer->display_list++, 0, 0, renderer->screen_size.x - 1, renderer->screen_size.y - 1);
    gDPPipeSync(renderer->display_list++);
}

void fw64_renderer_clear_frame_buffer(fw64Renderer* renderer) {
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b, renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxCfb_ptr));
    gDPSetFillColor(renderer->display_list++, (renderer->clear_color << 16 | renderer->clear_color));
    gDPFillRectangle(renderer->display_list++, 0, 0, renderer->screen_size.x - 1, renderer->screen_size.y - 1);
    gDPPipeSync(renderer->display_list++);
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE); 
}

void fw64_renderer_begin(fw64Renderer* renderer, fw64Camera* camera, fw64RenderMode render_mode, fw64RendererFlags flags) {
    renderer->render_mode = render_mode;
    renderer->camera = camera;
    renderer->flags = flags;

    if (flags & FW64_RENDERER_FLAG_CLEAR) {
        renderer->display_list = &renderer->gfx_list[0];
    }
    
    renderer->display_list_start = renderer->display_list;

    gSPSegment(renderer->display_list++, 0, 0x0);
    fw64_renderer_init_rdp(renderer);
    fw64_renderer_init_rsp(renderer);

    if (flags & FW64_RENDERER_FLAG_CLEAR) {
        fw64_renderer_clear_z_buffer(renderer);
        fw64_renderer_clear_frame_buffer(renderer);
    }

    // sets the projection matrix (modelling set in individual draw calls)
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
    gSPPerspNormalize(renderer->display_list++, camera->perspNorm);
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->view)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags) {
    gDPFullSync(renderer->display_list++);
    gSPEndDisplayList(renderer->display_list++);

    renderer->shading_mode = FW64_SHADING_MODE_UNSET;
    renderer->flags = FW64_RENDERER_FLAG_NONE;

    nuGfxTaskStart(renderer->display_list_start, 
        (s32)(renderer->display_list - renderer->display_list_start) * sizeof (Gfx), 
        (renderer->render_mode == FW64_RENDERER_MODE_LINES) ? NU_GFX_UCODE_L3DEX2 : NU_GFX_UCODE_F3DEX, 
        (flags & FW64_RENDERER_FLAG_SWAP) ? NU_SC_SWAPBUFFER : NU_SC_NOSWAPBUFFER);
}

static void fw64_renderer_set_shading_mode(fw64Renderer* renderer, fw64ShadingMode shading_mode) {
    if (renderer->shading_mode == shading_mode) return;

    renderer->shading_mode = shading_mode;

    switch(renderer->shading_mode) {
        case FW64_SHADING_MODE_UNLIT_TEXTURED:
            gDPSetRenderMode(renderer->display_list++, FW64_RM_3D_TEXTURED(renderer), FW64_RM_3D_TEXTURED2(renderer));
            gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
            gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
            gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);
            break;

        case FW64_SHADING_MODE_GOURAUD:
            gDPSetRenderMode(renderer->display_list++, FW64_RM_3D_OPAQUE_SHADED(renderer), FW64_RM_3D_OPAQUE_SHADED2(renderer));
            gSPSetGeometryMode(renderer->display_list++, G_LIGHTING | G_SHADE | G_SHADING_SMOOTH)
            #define G_CC_PRIM_SHADE PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0
            gDPSetCombineMode(renderer->display_list++, G_CC_PRIM_SHADE, G_CC_PRIM_SHADE);
            fw64_n64_renderer_set_lighting_data(renderer);
            break;

        case FW64_SHADING_MODE_GOURAUD_TEXTURED:
            gDPSetRenderMode(renderer->display_list++, FW64_RM_3D_TEXTURED(renderer), FW64_RM_3D_TEXTURED2(renderer));
            gSPSetGeometryMode(renderer->display_list++, G_LIGHTING | G_SHADE | G_SHADING_SMOOTH)
            gDPSetCombineMode(renderer->display_list++,G_CC_MODULATERGBA , G_CC_MODULATERGBA );
            fw64_n64_renderer_set_lighting_data(renderer);
            gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
            gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);
            break;

        case FW64_SHADING_MODE_SPRITE:
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE);
            gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
            gDPSetTexturePersp(renderer->display_list++, G_TP_NONE);
            break;

        case FW64_SHADING_MODE_UNLIT_VERTEX_COLORS:
            gDPSetRenderMode(renderer->display_list++, FW64_RM_3D_OPAQUE_SHADED(renderer), FW64_RM_3D_OPAQUE_SHADED2(renderer));
        break;

        default:
            break;
    }

    gDPPipeSync(renderer->display_list++);
}

void fw64_renderer_get_screen_size(fw64Renderer* renderer, IVec2* screen_size) {
    *screen_size = renderer->screen_size;
}

fw64Camera* fw64_renderer_get_camera(fw64Renderer* renderer) {
    return renderer->camera;
}

static void _fw64_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* sprite, int frame, int x, int y) {
    fw64_n64_renderer_load_texture(renderer, sprite, frame);

    gSPTextureRectangle(renderer->display_list++, 
            x << 2, y << 2, 
            (x + fw64_texture_slice_width(sprite)) << 2, (y + fw64_texture_slice_height(sprite)) << 2,
            G_TX_RENDERTILE, 
            0 << 5, 0 << 5, 
            1 << 10, 1 << 10);

    gDPPipeSync(renderer->display_list++);
}

void fw64_renderer_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* sprite, int frame, int x, int y) {
    fw64_renderer_set_shading_mode(renderer, FW64_SHADING_MODE_SPRITE);
    _fw64_draw_sprite_slice(renderer, sprite, frame, x, y);
}

void fw64_renderer_draw_sprite(fw64Renderer* renderer, fw64Texture* sprite, int x, int y) {
    fw64_renderer_set_shading_mode(renderer, FW64_SHADING_MODE_SPRITE);

    int slice_width = fw64_texture_slice_width(sprite);
    int slice_height = fw64_texture_slice_height(sprite);
    int slice = 0;

    for (uint8_t row = 0; row < sprite->image->info.vslices; row++ ) {
        int draw_y = y + row * slice_height;
        for (uint8_t col = 0; col < sprite->image->info.hslices; col++) {
            int draw_x = x + slice_width * col;

            _fw64_draw_sprite_slice(renderer, sprite, slice++, draw_x, draw_y);
        }
    }
}

void fw64_renderer_draw_text(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text) {
    if (!text || text[0] == 0) return;
    fw64_renderer_set_shading_mode(renderer, FW64_SHADING_MODE_SPRITE);
/*
    TODO: Color blending? need better alpha than 5551
    gDPSetPrimColor(renderer->display_list++, 255, 255, color->r, color->g, color->b, 255);
    gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGBA_PRIM , G_CC_MODULATERGBA_PRIM );
*/
    
    char ch = text[0];
    uint16_t glyph_index = fw64_font_get_glyph_index(font, ch);
    fw64FontGlyph* glyph = font->glyphs + glyph_index;
    uint16_t stride = font->spritefont_tile_width * font->spritefont_tile_height * 2;

    int caret = x + glyph->left;

    while (ch) {
        glyph_index = fw64_font_get_glyph_index(font, ch);
        glyph = font->glyphs + glyph_index;

        int draw_pos_x = caret + glyph->left;
        int draw_pos_y = y + glyph->top;

        gDPLoadTextureBlock(renderer->display_list++, font->spritefont + (stride * glyph_index), 
        G_IM_FMT_RGBA, G_IM_SIZ_16b, 
        font->spritefont_tile_width, font->spritefont_tile_height, 0, 
        G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gDPLoadSync(renderer->display_list++);

        gSPTextureRectangle(renderer->display_list++, 
            draw_pos_x << 2, draw_pos_y << 2, 
            (draw_pos_x + font->spritefont_tile_width) << 2, (draw_pos_y + font->spritefont_tile_height) << 2,
            G_TX_RENDERTILE, 
            0 << 5, 0 << 5, 
            1 << 10, 1 << 10);

        gDPPipeSync(renderer->display_list++);

        caret += glyph->advance;
        text++;
        ch = text[0];
    }
}

static void fw64_renderer_draw_primitive(fw64Renderer* renderer, fw64Mesh* mesh, fw64Primitive* primitive) {
        fw64_renderer_set_shading_mode(renderer, primitive->material->shading_mode);

        switch (primitive->material->shading_mode) {
            case FW64_SHADING_MODE_GOURAUD: {
                gDPSetPrimColor(renderer->display_list++, 0xFFFF, 0xFFFF, primitive->material->color.r, 
                    primitive->material->color.g, primitive->material->color.b, primitive->material->color.a);
            }
            break;

            case FW64_SHADING_MODE_GOURAUD_TEXTURED:
            case FW64_SHADING_MODE_UNLIT_TEXTURED: {
                fw64_n64_renderer_load_texture(renderer, primitive->material->texture, primitive->material->texture_frame);
            }
            break;

            default:
                break;
        }
            
        gSPDisplayList(renderer->display_list++, mesh->display_list + primitive->display_list);
        gDPPipeSync(renderer->display_list++);
}

void fw64_renderer_draw_static_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh) {
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&transform->matrix), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    
    for (uint32_t i = 0 ; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;
        
        fw64_renderer_draw_primitive(renderer, mesh, primitive);
    }

    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
}

void fw64_renderer_draw_animated_mesh(fw64Renderer* renderer, fw64Mesh* mesh, fw64AnimationController* controller, fw64Transform* transform) {
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&transform->matrix), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    
    for (uint32_t i = 0 ; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;

        gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(controller->matrices + primitive->joint_index), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
        
        fw64_renderer_draw_primitive(renderer, mesh, primitive);

        gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
    }

    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
}

void fw64_n64_renderer_load_texture(fw64Renderer* renderer, fw64Texture* texture, int frame) {
    fw64Image* image = texture->image;
    int slice_width = fw64_texture_slice_width(texture);
    int slice_height = fw64_texture_slice_height(texture);
    int frame_offset = slice_width * slice_height * image->info.bpp * frame;

    // unfortunetly due to the way that the gDPLoadTextureBlock macro is setup we have to pass G_IM_SIZ_16b in or G_IM_SIZ_32b
    switch (image->info.format)
    {
    case FW64_N64_IMAGE_FORMAT_RGBA16:
    gDPLoadTextureBlock(renderer->display_list++, texture->image->data + frame_offset,
        G_IM_FMT_RGBA, G_IM_SIZ_16b,  slice_width, slice_height, 0,
        texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
        break;

    case FW64_N64_IMAGE_FORMAT_RGBA32:
    gDPLoadTextureBlock(renderer->display_list++, texture->image->data + frame_offset,
        G_IM_FMT_RGBA, G_IM_SIZ_32b,  slice_width, slice_height, 0,
        texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
        break;
    
    default:
        break;
    }

    gDPLoadSync(renderer->display_list++);
}

// TODO: handle no active lights correctly...first light should be set to all black
static void fw64_n64_renderer_set_lighting_data(fw64Renderer* renderer) {
    int light_num = 1;
    int light_count = 0;
    if (renderer->active_light_mask & (1 << 0)) {
        gSPLight(renderer->display_list++, &renderer->lights.l[0], light_num++);
        light_count += 1;
    }

    if (renderer->active_light_mask & (1 << 1)) { 
        gSPLight(renderer->display_list++, &renderer->lights.l[1], light_num++);
        light_count += 1;
    }

    gSPLight(renderer->display_list++, &renderer->lights.a, light_num);
    gSPNumLights(renderer->display_list++, light_count);
}


void fw64_renderer_set_ambient_light_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->lights.a.l.col[0] = r;
    renderer->lights.a.l.col[1] = g;
    renderer->lights.a.l.col[2] = b;
    memcpy(&renderer->lights.a.l.colc[0], &renderer->lights.a.l.col[0], 3);
}

void fw64_renderer_set_light_enabled(fw64Renderer* renderer, int index, int enabled) {
    if (enabled) {
        renderer->active_light_mask |= (1 << index);
    }
    else {
        renderer->active_light_mask &= ~(1 << index);
    }
}

void fw64_renderer_set_light_direction(fw64Renderer* renderer, int index, float x, float y, float z) {
    Light* light = &renderer->lights.l[index];

    light->l.dir[0] = (uint8_t)(x * 100.0f);
    light->l.dir[1] = (uint8_t)(y * 100.0f);
    light->l.dir[2] = (uint8_t)(z * 100.0f);
}

void fw64_renderer_set_light_color(fw64Renderer* renderer, int index, uint8_t r, uint8_t g, uint8_t b) {
    Light* light = &renderer->lights.l[index];

    light->l.col[0] = r;
    light->l.col[1] = g;
    light->l.col[2] = b;

    memcpy(&light->l.colc[0], &light->l.col[0], 3);
}

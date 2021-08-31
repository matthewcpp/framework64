#include "framework64/renderer.h"

#include "framework64/matrix.h"

#include "framework64/n64/font.h"
#include "framework64/n64/mesh.h"
#include "framework64/n64/renderer.h"
#include "framework64/n64/texture.h"

#include <nusys.h>

#include <malloc.h>
#include <string.h>

void fw64_n64_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height) {
    renderer->screen_size.x = screen_width;
    renderer->screen_size.y = screen_height;
    renderer->display_list = NULL;

    renderer->fill_color = GPACK_RGBA5551(255, 255, 255, 1);
    renderer->clear_color = GPACK_RGBA5551(0, 0, 0, 1);

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

    renderer->render_mode = FW64_RENDERER_MODE_UNSET;
    renderer->shading_mode = FW64_SHADING_MODE_UNSET;
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

    renderer->display_list = &renderer->gfx_list[0];
    renderer->display_list_start = renderer->display_list;

    gSPSegment(renderer->display_list++, 0, 0x0);
    fw64_renderer_init_rdp(renderer);
    fw64_renderer_init_rsp(renderer);

    if (flags & FW64_RENDERER_FLAG_CLEAR) {
        fw64_renderer_clear_z_buffer(renderer);
        fw64_renderer_clear_frame_buffer(renderer);
    }

    switch (renderer->render_mode) {
        case FW64_RENDERER_MODE_UNSET:
            break;
        case FW64_RENDERER_MODE_TRIANGLES:
        case FW64_RENDERER_MODE_SPRITES:
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
        break;

        case FW64_RENDERER_MODE_LINES:
            gDPSetRenderMode(renderer->display_list++,  G_RM_AA_ZB_XLU_LINE,  G_RM_AA_ZB_XLU_LINE2);
        break;

        case FW64_RENDERER_MODE_RECTANGLES:
            gSPClearGeometryMode(renderer->display_list++, 0xFFFFFFFF);
        break;
    }

    // sets the projection matrix (modelling set in individual draw calls)
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
    gSPPerspNormalize(renderer->display_list++, camera->perspNorm);
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&(camera->view)), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
    
    gDPPipeSync(renderer->display_list++);
    
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags) {
    gDPFullSync(renderer->display_list++);
    gSPEndDisplayList(renderer->display_list++);

    renderer->shading_mode = FW64_SHADING_MODE_UNSET;

    nuGfxTaskStart(renderer->display_list_start, 
        (s32)(renderer->display_list - renderer->display_list_start) * sizeof (Gfx), 
        (renderer->render_mode == FW64_RENDERER_MODE_LINES) ? NU_GFX_UCODE_L3DEX2 : NU_GFX_UCODE_F3DEX, 
        (flags & FW64_RENDERER_FLAG_SWAP) ? NU_SC_SWAPBUFFER : NU_SC_NOSWAPBUFFER);
}

static void fw64_renderer_set_shading_mode(fw64Renderer* renderer, fw64ShadingMode shading_mode) {
    if (shading_mode == renderer->shading_mode) return;

    renderer->shading_mode = shading_mode;

    switch(renderer->shading_mode) {
        case FW64_SHADING_MODE_UNLIT_TEXTURED:
            gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);
            gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
            gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);
            break;

        case FW64_SHADING_MODE_GOURAUD:
            gSPSetGeometryMode(renderer->display_list++, G_LIGHTING)
            gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);
            break;

        case FW64_SHADING_MODE_GOURAUD_TEXTURED:
            gSPSetGeometryMode(renderer->display_list++, G_LIGHTING)
            gDPSetCombineMode(renderer->display_list++,G_CC_MODULATERGBA , G_CC_MODULATERGBA );
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);
            gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
            gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);
            gDPSetTextureFilter(renderer->display_list++,G_TF_POINT);
        break;

        case FW64_SHADING_MODE_SPRITE:
            gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE);
            gDPSetTexturePersp(renderer->display_list++, G_TP_NONE);
            gDPPipeSync(renderer->display_list++);
            break;
        default:
            break;
    }
}

void fw64_renderer_get_screen_size(fw64Renderer* renderer, IVec2* screen_size) {
    *screen_size = renderer->screen_size;
}

void fw64_renderer_set_fill_color(fw64Renderer* renderer, Color* color) {
    renderer->fill_color = GPACK_RGBA5551(color->r, color->g, color->b, 255);

    if (renderer->display_list) {
        gDPSetFillColor(renderer->display_list++, (renderer->fill_color << 16 | renderer->fill_color));
    }
}

void fw64_renderer_set_fill_mode(fw64Renderer* renderer) {
    gDPSetFillColor(renderer->display_list++, (renderer->fill_color << 16 | renderer->fill_color));
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);
    //TODO: do i need to sync pipe here?
}

void fw64_renderer_draw_filled_rect(fw64Renderer* renderer, IRect* rect) {
    gDPFillRectangle(renderer->display_list++, rect->x, rect->y, rect->x + rect->width, rect->y + rect->height);
    gDPPipeSync(renderer->display_list++);
}

static void _fw64_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* sprite, int frame, int x, int y) {
    int slice_width = fw64_texture_slice_width(sprite);
    int slice_height = fw64_texture_slice_height(sprite);

    uint32_t frame_offset = (slice_width * slice_height * 2) * frame;

    gDPLoadTextureBlock(renderer->display_list++, sprite->data + frame_offset, 
        G_IM_FMT_RGBA, G_IM_SIZ_16b, slice_width, slice_height, 0, 
        sprite->wrap_s, sprite->wrap_t, sprite->mask_s, sprite->mask_t, G_TX_NOLOD, G_TX_NOLOD);

    gDPLoadSync(renderer->display_list++);

    gSPTextureRectangle(renderer->display_list++, 
            x << 2, y << 2, 
            (x + slice_width) << 2, (y + slice_height) << 2,
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

    for (uint8_t row = 0; row < sprite->vslices; row++ ) {
        int draw_y = y + row * slice_height;
        for (uint8_t col = 0; col < sprite->hslices; col++) {
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

void fw64_renderer_draw_static_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh) {
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&transform->matrix), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    
    for (uint32_t i = 0 ; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;
        
        fw64_renderer_set_shading_mode(renderer, primitive->material.mode);

        if (primitive->material.mode == FW64_SHADING_MODE_GOURAUD_TEXTURED || 
            primitive->material.mode == FW64_SHADING_MODE_UNLIT_TEXTURED ) {
            fw64Texture* texture = mesh->textures + primitive->material.texture;

            int slice_width = fw64_texture_slice_width(texture);
            int slice_height = fw64_texture_slice_height(texture);
            int frame_offset = slice_width * slice_height * 2 * primitive->material.texture_frame;

            gDPLoadTextureBlock(renderer->display_list++, texture->data + frame_offset,
                G_IM_FMT_RGBA, G_IM_SIZ_16b,  slice_width, slice_height, 0,
                texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
        }

        if (primitive->material.color != FW64_MATERIAL_NO_COLOR)
            gSPSetLights1(renderer->display_list++, mesh->colors[primitive->material.color]);

        gSPDisplayList(renderer->display_list++, mesh->display_list + primitive->display_list);
        gDPPipeSync(renderer->display_list++);
    }

    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
}

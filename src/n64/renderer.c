#include "framework64/n64/renderer.h"

#include "framework64/matrix.h"

#include "framework64/n64/font.h"
#include "framework64/n64/framebuffer.h"
#include "framework64/n64/image.h"
#include "framework64/math.h"
#include "framework64/n64/mesh.h"
#include "framework64/n64/texture.h"

#include <nusys.h>

#include <malloc.h>
#include <string.h>


#define SET_RENDERER_FEATURE(renderer, feature) (renderer)->enabled_features |= (feature)
#define UNSET_RENDERER_FEATURE(renderer, feature) (renderer)->enabled_features &= ~(feature)
#define GET_RENDERER_FEATURE(renderer, feature) ((renderer)->enabled_features & (feature))

static void fw64_n64_renderer_update_lighting_data(fw64Renderer* renderer);

static void fw64_n64_renderer_load_texture(fw64Renderer* renderer, fw64Texture* texture, int frame);

void fw64_n64_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height) {
    renderer->screen_size.x = screen_width;
    renderer->screen_size.y = screen_height;

    renderer->display_list = NULL;
    renderer->active_texture = NULL;
    renderer->primitive_mode = FW64_PRIMITIVE_MODE_UNSET;
    renderer->shading_mode = FW64_SHADING_MODE_UNSET;
    renderer->flags = FW64_RENDERER_FLAG_NONE;

    n64_fill_rect_init(&renderer->fill_rect);
    renderer->clear_color = GPACK_RGBA5551(0, 0, 0, 1);

    renderer->enabled_features = N64_RENDERER_FEATURE_AA | N64_RENDERER_FEATURE_DEPTH_TEST;

    fw64_renderer_set_fog_positions(renderer, 0.4f, 0.8f);
    fw64_renderer_set_fog_color(renderer, 85, 85, 85);

    // set default lighting state
    Lights2 lights = gdSPDefLights2(
        85 , 85, 85,
        200, 200, 200, 40, 40, 40,
        200, 200, 200, -40, -40, -40
    );
    renderer->lights = lights;
    renderer->active_light_mask = 1;
    renderer->starting_new_frame = 1;

    renderer->post_draw_func = NULL;
    renderer->post_draw_func_arg = NULL;
}

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->clear_color = GPACK_RGBA5551(r, g, b, 1);
}

void fw64_renderer_set_post_draw_callback(fw64Renderer* renderer, fw64RendererPostDrawFunc func, void* arg) {
    renderer->post_draw_func = func;
    renderer->post_draw_func_arg = arg;
}

void fw64_n64_renderer_swap_func(fw64Renderer* renderer, NUScTask* gfxTaskPtr) {
    if (renderer->post_draw_func == NULL)
        return;

    fw64Framebuffer framebuffer;
    framebuffer.buffer = (u16*)gfxTaskPtr->framebuffer;
    framebuffer.width = renderer->screen_size.x;
    framebuffer.height = renderer->screen_size.y;

    renderer->post_draw_func(&framebuffer, renderer->post_draw_func_arg);
}

static Gfx _rdp_init_static_dl[] = {
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
    gsDPSetPrimColor(0xFFFF, 0xFFFF, 255, 255, 255, 255),
    gsSPEndDisplayList()
};

void fw64_renderer_init_rdp(fw64Renderer* renderer) {
    gDPPipelineMode(renderer->display_list++, G_PM_NPRIMITIVE); // experimental for increased RDP fill rate, default is G_PM_1PRIMITIVE
    gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE);
    gDPSetScissor(renderer->display_list++,G_SC_NON_INTERLACE, 0, 0, renderer->screen_size.x, renderer->screen_size.y);
    gSPDisplayList(renderer->display_list++, _rdp_init_static_dl);
    gDPSetDepthImage(renderer->display_list++,nuGfxZBuffer);
    gDPPipeSync(renderer->display_list++);
}


void fw64_renderer_init_rsp(fw64Renderer* renderer) {
    gSPClearGeometryMode(renderer->display_list++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF);
    gSPSetGeometryMode(renderer->display_list++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);
}

/** Note: This function assumes that the render mode is set to G_RM_OPA_SURF / G_RM_OPA_SURF2 
 *  After the call to this function the cycle type will be set to G_CYC_FILL.  It is the caller's responsibility to reset the correct cycle after the call to this function.
*/
void fw64_n64_renderer_clear_rect(fw64Renderer* renderer, int x, int y, int width, int height, fw64RendererFlags flags) {
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);

    if (flags & FW64_RENDERER_FLAG_CLEAR_DEPTH) {
        gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b,renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
        gDPSetFillColor(renderer->display_list++,(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
        gDPFillRectangle(renderer->display_list++, x, y, width - 1, height - 1);
        gDPPipeSync(renderer->display_list++);
    }

    gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b, renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxCfb_ptr));
    gDPSetFillColor(renderer->display_list++, (renderer->clear_color << 16 | renderer->clear_color));

    if (flags & FW64_RENDERER_FLAG_CLEAR_COLOR) {
        gDPFillRectangle(renderer->display_list++, x, y, width - 1, height - 1);
        gDPPipeSync(renderer->display_list++);
    }
}

IVec2 fw64_renderer_get_viewport_size(fw64Renderer* renderer, fw64Camera* camera) {
    IVec2 viewport_size = {
        (int)(renderer->screen_size.x * camera->viewport_size.x),
        (int)(renderer->screen_size.y * camera->viewport_size.y)
    };

    return viewport_size;
}

void fw64_renderer_set_camera(fw64Renderer* renderer, fw64Camera* camera) {
    renderer->camera = camera;

    float x = (renderer->screen_size.x * camera->viewport_pos.x);
    float y = (renderer->screen_size.y * camera->viewport_pos.y);
    float width = (renderer->screen_size.x * camera->viewport_size.x);
    float height = (renderer->screen_size.y * camera->viewport_size.y);

    camera->_viewport.vp.vscale[0] = (short)width * 2;
    camera->_viewport.vp.vscale[1] = (short)height * 2;
    camera->_viewport.vp.vscale[2] = G_MAXZ / 2;
    camera->_viewport.vp.vscale[3] = 0;

    camera->_viewport.vp.vtrans[0] = ((short)width * 2) + ((short)x * 4);
    camera->_viewport.vp.vtrans[1] = ((short)height * 2) + ((short)y * 4);
    camera->_viewport.vp.vtrans[2] = G_MAXZ / 2;
    camera->_viewport.vp.vtrans[3] = 0;

    gSPViewport(renderer->display_list++, &camera->_viewport);
    gDPSetScissor(renderer->display_list++, G_SC_NON_INTERLACE, x, y, x + width, y + height);

    renderer->viewport_screen_pos.x = (int)(renderer->screen_size.x * camera->viewport_pos.x);
    renderer->viewport_screen_pos.y = (int)(renderer->screen_size.y * camera->viewport_pos.y);

    // sets the view projection matrices  This is slightly counter intuitive
    // Refer to: 11.7.3.1 Important Note on Matrix Manipulation in the N64 Programming manual on ultra64.ca
    gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&(camera->projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
    gSPPerspNormalize(renderer->display_list++, camera->_persp_norm);
    gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&(camera->view)), G_MTX_PROJECTION|G_MTX_MUL|G_MTX_NOPUSH);
}

void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64RendererFlags flags) {
    renderer->primitive_mode = primitive_mode;
    renderer->active_texture = NULL;
    
    renderer->flags = flags;

    if (renderer->starting_new_frame) {
        renderer->display_list = &renderer->gfx_list[0];
    }
    
    renderer->display_list_start = renderer->display_list;

    gSPSegment(renderer->display_list++, 0, 0x0);
    fw64_renderer_init_rdp(renderer);
    fw64_renderer_init_rsp(renderer);

    if (flags & FW64_RENDERER_FLAG_CLEAR) {
        fw64_n64_renderer_clear_rect(renderer, 0, 0, renderer->screen_size.x, renderer->screen_size.y, flags);
    }

    fw64_n64_configure_fog(renderer, GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG));
    gSPSetLights2(renderer->display_list++, renderer->lights);
}

void fw64_n64_configure_fog(fw64Renderer* renderer, int enabled) {
    if (enabled) {
        gDPSetCycleType(renderer->display_list++, G_CYC_2CYCLE);
        gDPSetFogColor(renderer->display_list++, renderer->fog_color.r, renderer->fog_color.g, renderer->fog_color.b, renderer->fog_color.a);
        gSPFogPosition(renderer->display_list++, renderer->fog_min, renderer->fog_max);
        gSPSetGeometryMode(renderer->display_list++, G_FOG);
    }
    else {
        gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE);
        gSPClearGeometryMode(renderer->display_list++, G_FOG);
    }
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags) {
    gDPFullSync(renderer->display_list++);
    gSPEndDisplayList(renderer->display_list++);

    renderer->shading_mode = FW64_SHADING_MODE_UNSET;
    renderer->flags = FW64_RENDERER_FLAG_NONE;

    nuGfxTaskStart(renderer->display_list_start, 
        (s32)(renderer->display_list - renderer->display_list_start) * sizeof (Gfx), 
        (renderer->primitive_mode == FW64_PRIMITIVE_MODE_LINES) ? NU_GFX_UCODE_L3DEX2 : NU_GFX_UCODE_F3DEX, 
        (flags & FW64_RENDERER_FLAG_SWAP) ? NU_SC_SWAPBUFFER : NU_SC_NOSWAPBUFFER);
}

static inline void n64_renderer_set_render_mode_opaque(fw64Renderer* renderer) {
    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG)) {
        gDPSetRenderMode(renderer->display_list++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
    }
    else {
        gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF);
    }
}

/**  Color = vertex color * material color*/
static void n64_renderer_configure_shading_mode_vertex_color(fw64Renderer* renderer, fw64Material* material) {
    if (renderer->shading_mode == FW64_SHADING_MODE_VERTEX_COLOR)
        return;

    
    n64_renderer_set_render_mode_opaque(renderer);
    gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);

    gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF );

    renderer->shading_mode = FW64_SHADING_MODE_VERTEX_COLOR;
}

/**  Color = vertex color * texture color*/
static void n64_renderer_configure_shading_mode_vertex_color_textured(fw64Renderer* renderer, fw64Material* material) {
    fw64_n64_renderer_load_texture(renderer, material->texture, material->texture_frame);

    if (renderer->shading_mode == FW64_SHADING_MODE_VERTEX_COLOR_TEXTURED)
        return;

    n64_renderer_set_render_mode_opaque(renderer);
    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG)){
        gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGB, G_CC_MODULATERGB2);
    }
    else {
        gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGB, G_CC_MODULATERGB);
    }

    
    gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

    renderer->shading_mode = FW64_SHADING_MODE_VERTEX_COLOR_TEXTURED;
}

/**  Color = texture color * material color.  No ligthing calculations  */
static void n64_renderer_configure_shading_mode_unlit_textured(fw64Renderer* renderer, fw64Material* material) {
    fw64_n64_renderer_load_texture(renderer, material->texture, material->texture_frame);

    if (renderer->shading_mode == FW64_SHADING_MODE_UNLIT_TEXTURED)
        return;

    // todo: opaque vs transparent?

if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG)){
    gDPSetRenderMode(renderer->display_list++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_PASS2);
}
else{
    gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF);
    gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_DECALRGBA);
}
    gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
    gSPSetGeometryMode(renderer->display_list++, G_SHADE | G_SHADING_SMOOTH);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

    renderer->shading_mode = FW64_SHADING_MODE_UNLIT_TEXTURED;
}

#define PACK_LIGHT_VAL(r, g, b) (u32)(r << 24) | (g << 16) | (b << 8) | 255

/** Color = shade * primitive color  */ 
static void n64_renderer_configure_shading_mode_gouraud(fw64Renderer* renderer, fw64Material* material) {
    gSPLightColor(renderer->display_list++, LIGHT_1, PACK_LIGHT_VAL(material->color.r, material->color.g, material->color.b));
    gSPLightColor(renderer->display_list++, LIGHT_2, PACK_LIGHT_VAL(material->color.r, material->color.g, material->color.b));

    if (renderer->shading_mode == FW64_SHADING_MODE_GOURAUD)
        return;

    n64_renderer_set_render_mode_opaque(renderer);
    gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);

    gSPSetGeometryMode(renderer->display_list++, G_LIGHTING | G_SHADE | G_SHADING_SMOOTH);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF );

    renderer->shading_mode = FW64_SHADING_MODE_GOURAUD;
}


/** Color = shade * texture color  */ 
static void n64_renderer_configure_shading_mode_gouraud_textured(fw64Renderer* renderer, fw64Material* material) {
    fw64_n64_renderer_load_texture(renderer, material->texture, material->texture_frame);

    if (renderer->shading_mode == FW64_SHADING_MODE_GOURAUD_TEXTURED)
        return;

    gSPLightColor(renderer->display_list++, LIGHT_1, UINT32_MAX);
    gSPLightColor(renderer->display_list++, LIGHT_2, UINT32_MAX);
    n64_renderer_set_render_mode_opaque(renderer);
    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG)){
        gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGB, G_CC_MODULATERGB2);
    }
    else {
        gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGB, G_CC_MODULATERGB);
    }

    gSPSetGeometryMode(renderer->display_list++, G_LIGHTING | G_SHADE | G_SHADING_SMOOTH);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

    renderer->shading_mode = FW64_SHADING_MODE_GOURAUD_TEXTURED;
}

static void n64_renderer_configure_shading_mode_decal_texture(fw64Renderer* renderer, fw64Material* material) {
    fw64_n64_renderer_load_texture(renderer, material->texture, material->texture_frame);

    if (renderer->shading_mode == FW64_SHADING_MODE_DECAL_TEXTURE)
        return;

    gDPSetRenderMode(renderer->display_list++, G_RM_ZB_XLU_DECAL, G_RM_ZB_XLU_DECAL2);
    gDPSetCombineMode(renderer->display_list++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

    renderer->shading_mode = FW64_SHADING_MODE_DECAL_TEXTURE;
}

static void n64_renderer_configure_shading_mode_sprite(fw64Renderer* renderer) {
    if (renderer->shading_mode == FW64_SHADING_MODE_SPRITE)
        return;

    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG))
        fw64_n64_configure_fog(renderer, 0);

#define	FwW64_N64_CC_SPRITE	TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0

    gDPSetRenderMode(renderer->display_list++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(renderer->display_list++, FwW64_N64_CC_SPRITE, FwW64_N64_CC_SPRITE); // sprite
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_NONE);
    gDPPipeSync(renderer->display_list++);

    renderer->shading_mode = FW64_SHADING_MODE_SPRITE;
}

static void n64_renderer_configure_shading_mode_fill(fw64Renderer* renderer) {
    if (renderer->shading_mode == FW64_SHADING_MODE_FILL)
        return;

    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG))
        fw64_n64_configure_fog(renderer, 0);

    gDPSetRenderMode(renderer->display_list++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGB_PRIM, G_CC_MODULATERGB_PRIM);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_NONE);
    gDPPipeSync(renderer->display_list++);

    renderer->shading_mode = FW64_SHADING_MODE_FILL;
}

void fw64_renderer_set_fill_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    gDPSetPrimColor(renderer->display_list++, 0xFFFF, 0xFFFF, r, g, b, a);
}

static void n64_renderer_configure_mesh_shading_mode(fw64Renderer* renderer, fw64Material* material) {
    switch(material->shading_mode) {
        case FW64_SHADING_MODE_VERTEX_COLOR:
            n64_renderer_configure_shading_mode_vertex_color(renderer, material);
            break;

        case FW64_SHADING_MODE_VERTEX_COLOR_TEXTURED:
            n64_renderer_configure_shading_mode_vertex_color_textured(renderer, material);
            break;

        case FW64_SHADING_MODE_GOURAUD:
            n64_renderer_configure_shading_mode_gouraud(renderer, material);
            break;

        case FW64_SHADING_MODE_GOURAUD_TEXTURED:
            n64_renderer_configure_shading_mode_gouraud_textured(renderer, material);
            break;

        case FW64_SHADING_MODE_UNLIT_TEXTURED:
            n64_renderer_configure_shading_mode_unlit_textured(renderer, material);
            break;

        case FW64_SHADING_MODE_DECAL_TEXTURE:
            n64_renderer_configure_shading_mode_decal_texture(renderer, material);
            break;
    }
}

void fw64_renderer_get_screen_size(fw64Renderer* renderer, IVec2* screen_size) {
    *screen_size = renderer->screen_size;
}

fw64Camera* fw64_renderer_get_camera(fw64Renderer* renderer) {
    return renderer->camera;
}

static void _fw64_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* sprite, int frame, int x, int y, int width, int height) {
    fw64_n64_renderer_load_texture(renderer, sprite, frame);

    x = renderer->viewport_screen_pos.x + x;
    y = renderer->viewport_screen_pos.y + y;

    gSPTextureRectangle(renderer->display_list++, 
            x << 2, y << 2, 
            (x + width) << 2, (y + height) << 2,
            G_TX_RENDERTILE, 
            0 << 5, 0 << 5, 
            1 << 10, 1 << 10);
}

void fw64_renderer_draw_filled_rect(fw64Renderer* renderer, int x, int y, int width, int height) {
    n64_renderer_configure_shading_mode_fill(renderer);
    _fw64_draw_sprite_slice(renderer, &renderer->fill_rect.texture, 0, x, y, width, height);
}

void fw64_renderer_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* sprite, int frame, int x, int y) {
    n64_renderer_configure_shading_mode_sprite(renderer);
    _fw64_draw_sprite_slice(renderer, sprite, frame, x, y, fw64_texture_slice_width(sprite), fw64_texture_slice_height(sprite));
}

void fw64_renderer_draw_sprite_slice_transform(fw64Renderer* renderer, fw64Texture* texture, int frame, int x, int y, float scale_x , float scale_y, float rotation) {
    float width = (float)fw64_texture_slice_width(texture) * scale_x;
    float height = (float)fw64_texture_slice_height(texture) * scale_y;

    n64_renderer_configure_shading_mode_sprite(renderer);
    _fw64_draw_sprite_slice(renderer, texture, frame, x, y, (int)width, (int)height);
}

void fw64_renderer_draw_sprite(fw64Renderer* renderer, fw64Texture* sprite, int x, int y) {
    n64_renderer_configure_shading_mode_sprite(renderer);

    int slice_width = fw64_texture_slice_width(sprite);
    int slice_height = fw64_texture_slice_height(sprite);
    int slice = 0;

    for (uint8_t row = 0; row < sprite->image->info.vslices; row++ ) {
        int draw_y = y + row * slice_height;
        for (uint8_t col = 0; col < sprite->image->info.hslices; col++) {
            int draw_x = x + slice_width * col;

            _fw64_draw_sprite_slice(renderer, sprite, slice++, draw_x, draw_y, slice_width, slice_height);
        }
    }
}

void fw64_renderer_draw_text(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text) {
    fw64_renderer_draw_text_count(renderer, font, x, y, text, UINT32_MAX);
}

void fw64_renderer_draw_text_count(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text, uint32_t count) {
    if (!text || text[0] == 0) return;
    n64_renderer_configure_shading_mode_sprite(renderer);

    x = renderer->viewport_screen_pos.x + x;
    y = renderer->viewport_screen_pos.y + y;
    
    char ch = text[0];
    uint16_t glyph_index = fw64_font_get_glyph_index(font, ch);
    fw64FontGlyph* glyph = font->glyphs + glyph_index;
    fw64Texture* texture = &font->texture;
    int spritefont_tile_width = fw64_texture_slice_width(texture);
    int spritefont_tile_height = fw64_texture_slice_height(texture);

    int caret = x + glyph->left;

    for (uint32_t i = 0; i < count && ch != 0; i++) {
        glyph_index = fw64_font_get_glyph_index(font, ch);
        glyph = font->glyphs + glyph_index;

        int draw_pos_x = caret + glyph->left;
        int draw_pos_y = y + glyph->top;

        fw64_n64_renderer_load_texture(renderer, texture, glyph_index);

        gSPTextureRectangle(renderer->display_list++, 
            draw_pos_x << 2, draw_pos_y << 2, 
            (draw_pos_x + spritefont_tile_width) << 2, (draw_pos_y + spritefont_tile_height) << 2,
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
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&transform->matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
    
    for (uint32_t i = 0 ; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;
        
        n64_renderer_configure_mesh_shading_mode(renderer, primitive->material);
        gSPDisplayList(renderer->display_list++, primitive->display_list);
        gDPPipeSync(renderer->display_list++);
    }

    // note: pop is not necessary here...we are simply overwriting the MODELVIEW matrix see note above in set_camera
    //gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
}

void fw64_renderer_draw_animated_mesh(fw64Renderer* renderer, fw64Mesh* mesh, fw64AnimationController* controller, fw64Transform* transform) {
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&transform->matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
    
    for (uint32_t i = 0 ; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;

        gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(controller->matrices + primitive->joint_index), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
        
        n64_renderer_configure_mesh_shading_mode(renderer, primitive->material);
        gSPDisplayList(renderer->display_list++, primitive->display_list);
        gDPPipeSync(renderer->display_list++);

        gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
    }

    // note: pop is not necessary here...we are simply overwriting the MODELVIEW matrix see note above in set_camera
    //gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
}

void fw64_n64_renderer_load_texture(fw64Renderer* renderer, fw64Texture* texture, int frame) {
    if (texture == renderer->active_texture && frame == renderer->active_texture_frame)
        return;

    fw64Image* image = texture->image;
    uint8_t* image_data = fw64_n64_image_get_data(image, frame);
    int slice_width = fw64_texture_slice_width(texture);
    int slice_height = fw64_texture_slice_height(texture);

    // unfortunetly due to the way that the gDPLoadTextureBlock macro is setup we have to pass G_IM_SIZ_16b in or G_IM_SIZ_32b
    switch (image->info.format)
    {
    case FW64_N64_IMAGE_FORMAT_RGBA16:
        gDPLoadTextureBlock(renderer->display_list++, image_data,
            G_IM_FMT_RGBA, G_IM_SIZ_16b,  slice_width, slice_height, 0,
            texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
            break;

    case FW64_N64_IMAGE_FORMAT_RGBA32:
        gDPLoadTextureBlock(renderer->display_list++, image_data,
            G_IM_FMT_RGBA, G_IM_SIZ_32b,  slice_width, slice_height, 0,
            texture->wrap_s, texture->wrap_t, texture->mask_s, texture->mask_t, G_TX_NOLOD, G_TX_NOLOD);
            break;
    
    default:
        break;
    }

    renderer->active_texture = texture;
    renderer->active_texture_frame = frame;
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

void fw64_renderer_set_fog_enabled(fw64Renderer* renderer, int enabled) {
    int fog_currently_enabled = fw64_renderer_get_fog_enabled(renderer);

    if (fog_currently_enabled == enabled)
        return;

    if (enabled)
        SET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG);
    else
        UNSET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG);

    if (renderer->primitive_mode != FW64_PRIMITIVE_MODE_UNSET)
        fw64_n64_configure_fog(renderer, enabled);
}

int fw64_renderer_get_fog_enabled(fw64Renderer* renderer) {
    return renderer->enabled_features & N64_RENDERER_FEATURE_FOG;
}

/**
 * The N64 Fog Algorithm is not totally clear.
 * This implementation attempts to provide a reasonable approximation of how I think it should work.
 * Note that a crash has been observed if fog_min == fog_max
 */
void fw64_renderer_set_fog_positions(fw64Renderer* renderer, float fog_min, float fog_max) {
    renderer->fog_min = (s32)fw64_clamp(900.0f + (fog_min * 100.0f), 0.0f, 1000.0f);
    renderer->fog_max = (s32)fw64_clamp(900.0f + (fog_max * 100.0f), 0.0f, 1000.0f);

    if (renderer->fog_min == renderer->fog_max) {
        renderer->fog_min = renderer->fog_max - 8;
    }
}

void fw64_renderer_set_fog_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    fw64_color_rgba8_set(&renderer->fog_color, r, g, b, 255);
}

void fw64_renderer_set_depth_testing_enabled(fw64Renderer* renderer, int enabled) {
    if (enabled)
        SET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_DEPTH_TEST);
    else
        UNSET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_DEPTH_TEST);
}

int fw64_renderer_get_depth_testing_enabled(fw64Renderer* renderer) {
    return GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_DEPTH_TEST);
}

void fw64_renderer_set_anti_aliasing_enabled(fw64Renderer* renderer, int enabled){
    if (enabled)
        SET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_AA);
    else
        UNSET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_AA);
}

int fw64_renderer_get_anti_aliasing_enabled(fw64Renderer* renderer) {
    return GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_AA);
}

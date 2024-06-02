#include "framework64/n64/renderer.h"

#include "framework64/matrix.h"

#include "framework64/n64/font.h"
#include "framework64/n64/image.h"
#include "framework64/math.h"
#include "framework64/n64/mesh.h"
#include "framework64/n64/sprite_batch.h"
#include "framework64/n64/texture.h"

#include <nusys.h>

#include <malloc.h>
#include <string.h>

static void fw64_n64_renderer_update_lighting_data(fw64Renderer* renderer);

void fw64_n64_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height) {
    renderer->screen_size.x = screen_width;
    renderer->screen_size.y = screen_height;

    renderer->display_list = NULL;
    renderer->primitive_mode = FW64_PRIMITIVE_MODE_UNSET;
    renderer->shading_mode = FW64_SHADING_MODE_UNSET;
    renderer->flags = FW64_RENDERER_FLAG_NONE;

    n64_fill_rect_init(&renderer->fill_rect);
    renderer->clear_color = GPACK_RGBA5551(0, 0, 0, 1);

    guMtxIdent(&renderer->identity_matrix);

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
}

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->clear_color = GPACK_RGBA5551(r, g, b, 1);
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
void fw64_n64_renderer_clear_rect(fw64Renderer* renderer, int x, int y, int width, int height, u16 clear_color, fw64ClearFlags flags) {
    int right = x + width - 1;
    int bottom = y + height - 1;

    gDPSetRenderMode(renderer->display_list++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(renderer->display_list++, G_CYC_FILL);

    if (flags & FW64_CLEAR_FLAG_DEPTH) {
        gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b,renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxZBuffer));
        gDPSetFillColor(renderer->display_list++,(GPACK_ZDZ(G_MAXFBZ,0) << 16 | GPACK_ZDZ(G_MAXFBZ,0)));
        gDPFillRectangle(renderer->display_list++, x, y, right, bottom);
        gDPPipeSync(renderer->display_list++);
    }

    gDPSetColorImage(renderer->display_list++, G_IM_FMT_RGBA, G_IM_SIZ_16b, renderer->screen_size.x, OS_K0_TO_PHYSICAL(nuGfxCfb_ptr));
    gDPSetFillColor(renderer->display_list++, (clear_color << 16 | clear_color));

    if (flags & FW64_CLEAR_FLAG_COLOR) {
        gDPFillRectangle(renderer->display_list++, x, y, right, bottom);
        gDPPipeSync(renderer->display_list++);
    }
}

void fw64_n64_renderer_clear_viewport(fw64Renderer* renderer, fw64Viewport* viewport, u16 clear_color, fw64ClearFlags flags) {
    gDPPipeSync(renderer->display_list++);
    fw64_n64_renderer_clear_rect(renderer, viewport->position.x, viewport->position.y, viewport->size.x, viewport->size.y, clear_color, flags);
    gDPPipeSync(renderer->display_list++);
    fw64_n64_configure_fog(renderer, fw64_renderer_get_fog_enabled(renderer));
    renderer->shading_mode = FW64_SHADING_MODE_UNSET;
}

IVec2 fw64_renderer_get_viewport_size(fw64Renderer* renderer, fw64Camera* camera) {
    IVec2 viewport_size = {
        (int)(renderer->screen_size.x * camera->viewport.size.x),
        (int)(renderer->screen_size.y * camera->viewport.size.y)
    };

    return viewport_size;
}

static void fw64_renderer_load_matrices(fw64Renderer* renderer, fw64Matrix* projection, uint16_t* persp_norm_ptr, fw64Matrix* view, int setupForLighting) {
    gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(projection), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
    uint16_t persp_norm = (persp_norm_ptr) ? *persp_norm_ptr : FW64_N64_LIBULTRA_DEFAULT_PERSPNORM;
    gSPPerspNormalize(renderer->display_list++, persp_norm);

    if (setupForLighting) {
        // sets the view projection matrices  This is slightly counter intuitive
        // Refer to: 11.7.3.1 Important Note on Matrix Manipulation in the N64 Programming manual on ultra64.ca
        gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(view), G_MTX_PROJECTION|G_MTX_MUL|G_MTX_NOPUSH);
    } else {
        gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(view), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
    }
}

void fw64_renderer_set_view_matrices(fw64Renderer* renderer, fw64Matrix* projection, uint16_t* persp_norm_ptr, fw64Matrix* view) {
    // TODO: fw64_renderer_set_view_matrices is going away
    fw64_renderer_load_matrices(renderer, projection, persp_norm_ptr, view, 1);
}

void fw64_renderer_set_viewport(fw64Renderer* renderer, fw64Viewport* viewport) {
    gSPViewport(renderer->display_list++, &viewport->_n64_viewport);

    //TODO: is this needed?
    // gDPSetScissor(renderer->display_list++, G_SC_NON_INTERLACE, 
    //     viewport->position.x, viewport->position.y, 
    //     viewport->position.x + viewport->size.x, viewport->position.y + viewport->size.y);
}

void fw64_renderer_set_camera(fw64Renderer* renderer, fw64Camera* camera) {
    renderer->camera = camera;

    fw64_renderer_set_viewport(renderer, &camera->viewport);
    fw64_renderer_set_view_matrices(renderer, &camera->projection, &camera->_persp_norm, &camera->view);
}

void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64RendererFlags flags) {
    renderer->primitive_mode = primitive_mode;
    fw64_texture_state_default(&renderer->active_texture);
    
    renderer->flags = flags;

    if (renderer->starting_new_frame) {
        renderer->display_list = &renderer->gfx_list[0];
    }
    
    renderer->display_list_start = renderer->display_list;

    gSPSegment(renderer->display_list++, 0, 0x0);
    fw64_renderer_init_rdp(renderer);
    fw64_renderer_init_rsp(renderer);

    if (flags & FW64_RENDERER_FLAG_CLEAR) {
        fw64_n64_renderer_clear_rect(renderer, 0, 0, renderer->screen_size.x, renderer->screen_size.y, renderer->clear_color, (fw64ClearFlags)flags);
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
static void n64_renderer_configure_shading_mode_vertex_color(fw64Renderer* renderer) {
    if (renderer->shading_mode == FW64_SHADING_MODE_VERTEX_COLOR)
        return;

    n64_renderer_set_render_mode_opaque(renderer);
    gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);

    gSPSetGeometryMode(renderer->display_list++, G_CULL_BACK);
    gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF );

    renderer->shading_mode = FW64_SHADING_MODE_VERTEX_COLOR;
}

static void n64_renderer_set_shading_mode_vertex_color_textured(fw64Renderer* renderer) {
    if (renderer->shading_mode == FW64_SHADING_MODE_VERTEX_COLOR_TEXTURED)
        return;

    n64_renderer_set_render_mode_opaque(renderer);
    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG)){
        gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGB, G_CC_MODULATERGB2);
    }
    else {
        gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGB, G_CC_MODULATERGB);
    }

    gSPSetGeometryMode(renderer->display_list++, G_CULL_BACK);
    gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

    renderer->shading_mode = FW64_SHADING_MODE_VERTEX_COLOR_TEXTURED;
}

/**  Color = vertex color * texture color*/
static void n64_renderer_configure_shading_mode_vertex_color_textured(fw64Renderer* renderer, fw64Material* material) {
    renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, material->texture, material->texture_frame);

    n64_renderer_set_shading_mode_vertex_color_textured(renderer);
}

static void n64_renderer_set_shading_mode_unlit_textured(fw64Renderer* renderer) {
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

    gSPClearGeometryMode(renderer->display_list++, G_LIGHTING | G_CULL_BOTH);
    gSPSetGeometryMode(renderer->display_list++, G_SHADE | G_SHADING_SMOOTH);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

    renderer->shading_mode = FW64_SHADING_MODE_UNLIT_TEXTURED;
}

/**  Color = texture color * material color.  No ligthing calculations  */
static void n64_renderer_configure_shading_mode_unlit_textured(fw64Renderer* renderer, fw64Material* material) {
    renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, material->texture, material->texture_frame);

    n64_renderer_set_shading_mode_unlit_textured(renderer);
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

    gSPSetGeometryMode(renderer->display_list++, G_LIGHTING | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF );

    renderer->shading_mode = FW64_SHADING_MODE_GOURAUD;
}


/** Color = shade * texture color  */ 
static void n64_renderer_configure_shading_mode_gouraud_textured(fw64Renderer* renderer, fw64Material* material) {
    renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, material->texture, material->texture_frame);

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

    gSPSetGeometryMode(renderer->display_list++, G_LIGHTING | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

    renderer->shading_mode = FW64_SHADING_MODE_GOURAUD_TEXTURED;
}

static void n64_renderer_configure_shading_mode_decal_texture(fw64Renderer* renderer, fw64Material* material) {
    renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, material->texture, material->texture_frame);
    gDPSetPrimColor(renderer->display_list++, 0xFFFF, 0xFFFF, material->color.r, material->color.g, material->color.b, material->color.a);

    if (renderer->shading_mode == FW64_SHADING_MODE_DECAL_TEXTURE)
        return;

    #define	FwW64_N64_CC_DECAL_TEX	TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0

    gDPSetRenderMode(renderer->display_list++, G_RM_ZB_XLU_DECAL, G_RM_ZB_XLU_DECAL2);
    gDPSetCombineMode(renderer->display_list++, FwW64_N64_CC_DECAL_TEX, FwW64_N64_CC_DECAL_TEX);
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
            n64_renderer_configure_shading_mode_vertex_color(renderer);
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

IVec2 fw64_renderer_get_screen_size(fw64Renderer* renderer) {
    return renderer->screen_size;
}

fw64Camera* fw64_renderer_get_camera(fw64Renderer* renderer) {
    return renderer->camera;
}

void fw64_renderer_set_sprite_scissoring_enabled(fw64Renderer* renderer, int enabled) {
    if (enabled) {
        SET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_SPRITE_SCISSOR);
    }
    else {
        UNSET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_SPRITE_SCISSOR);
    }
}

int fw64_renderer_get_sprite_scissoring_enabled(fw64Renderer* renderer) {
    return GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_SPRITE_SCISSOR);
}

static void _fw64_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* sprite, int frame, int x, int y, int width, int height) {
    renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, sprite, frame);

    x += renderer->camera->viewport.position.x;
    y += renderer->camera->viewport.position.y;

    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_SPRITE_SCISSOR)) {
        gSPScisTextureRectangle (renderer->display_list++, 
            x << 2, y << 2, 
            (x + width) << 2, (y + height) << 2,
            G_TX_RENDERTILE, 
            0 << 5, 0 << 5, 
            1 << 10, 1 << 10);
    }
    else {
        gSPTextureRectangle(renderer->display_list++, 
            x << 2, y << 2, 
            (x + width) << 2, (y + height) << 2,
            G_TX_RENDERTILE, 
            0 << 5, 0 << 5, 
            1 << 10, 1 << 10);
    }
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

    x += renderer->camera->viewport.position.x;
    y += renderer->camera->viewport.position.y;
    
    char ch = text[0];
    uint16_t glyph_index = fw64_n64_font_get_glyph_index(font, ch);
    fw64FontGlyph* glyph = font->glyphs + glyph_index;
    fw64Texture* texture = &font->texture;
    int spritefont_tile_width = fw64_texture_slice_width(texture);
    int spritefont_tile_height = fw64_texture_slice_height(texture);

    int caret = x + glyph->left;

    for (uint32_t i = 0; i < count && ch != 0; i++) {
        glyph_index = fw64_n64_font_get_glyph_index(font, ch);
        glyph = font->glyphs + glyph_index;

        int draw_pos_x = caret + glyph->left;
        int draw_pos_y = y + glyph->top;

        renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, texture, glyph_index);

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

static void set_shading_mode_text(fw64Renderer* renderer) {
    gDPSetRenderMode(renderer->display_list++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPClearGeometryMode(renderer->display_list++, G_ZBUFFER | G_LIGHTING | G_CULL_BOTH);
    gDPSetCombineMode(renderer->display_list++, G_CC_MODULATEIDECALA , G_CC_MODULATEIDECALA );

    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

    gDPPipeSync(renderer->display_list++);
}

static void fw64_renderer_draw_texture_batch(fw64Renderer* renderer, fw64N64TextureBatch* texture_batch) {
    for (size_t l = 0; l < texture_batch->layer_count; l++) {
        fw64TextureBatchLayer* layer = texture_batch->layers + l;

        for (uint32_t i = 0; i < layer->batch_count; i++) {
            fw64N64QuadBatch* char_batch = layer->batches[i];
            gSPDisplayList(renderer->display_list++, char_batch->display_list);
            gDPPipeSync(renderer->display_list++);
        }
    }
}

void fw64_renderer_submit_renderpass(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    fw64_renderer_set_viewport(renderer, &renderpass->viewport);

    if (renderpass->clear_flags) {
        fw64_n64_renderer_clear_viewport(renderer, &renderpass->viewport, renderpass->clear_color, renderpass->clear_flags);
    }

    renderer->enabled_features = renderpass->enabled_features;

    if (!fw64_dynamic_vector_is_empty(&renderpass->render_queue.static_meshes)) {
        // TODO: This will likely change to only be set for goraud shaded objects
        fw64_renderer_load_matrices(renderer, &renderpass->projection_matrix, &renderpass->persp_norm, &renderpass->view_matrix, 1);

        for (size_t i = 0; i < fw64_dynamic_vector_size(&renderpass->render_queue.static_meshes); i++) {
            fw64N64MeshInstance* mesh_instance = (fw64N64MeshInstance*)fw64_dynamic_vector_item(&renderpass->render_queue.static_meshes, i);
            fw64_renderer_draw_static_mesh(renderer, mesh_instance->transform, mesh_instance->mesh);
        }
    }

    if (!fw64_dynamic_vector_is_empty(&renderpass->render_queue.sprite_batches)) {
        fw64_renderer_load_matrices(renderer, &renderpass->projection_matrix, &renderpass->persp_norm, &renderpass->view_matrix, 0);
        set_shading_mode_text(renderer);

        for (size_t i = 0; i < fw64_dynamic_vector_size(&renderpass->render_queue.sprite_batches); i++){
            fw64SpriteBatch* sprite_batch = *((fw64SpriteBatch**)fw64_dynamic_vector_item(&renderpass->render_queue.sprite_batches, i));
            fw64_renderer_draw_texture_batch(renderer, &sprite_batch->texture_batch);
        }
    }
}
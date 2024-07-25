#include "framework64/n64_libultra/renderer.h"

#include "framework64/matrix.h"

#include "framework64/n64_libultra/font.h"
#include "framework64/n64_libultra/image.h"
#include "framework64/math.h"
#include "framework64/n64_libultra/mesh.h"
#include "framework64/n64_libultra/sprite_batch.h"
#include "framework64/n64_libultra/texture.h"

#include <nusys.h>

#include <malloc.h>
#include <string.h>

void fw64_n64_renderer_init(fw64Renderer* renderer, int screen_width, int screen_height) {
    renderer->screen_size.x = screen_width;
    renderer->screen_size.y = screen_height;

    renderer->display_list = NULL;
    renderer->primitive_mode = FW64_PRIMITIVE_MODE_UNSET;
    renderer->shading_mode = FW64_SHADING_MODE_UNSET;

    n64_fill_rect_init(&renderer->fill_rect);
    renderer->clear_color = GPACK_RGBA5551(0, 0, 0, 1);

    guMtxIdent(&renderer->identity_matrix);

    renderer->enabled_features = N64_RENDERER_FEATURE_AA | N64_RENDERER_FEATURE_DEPTH_TEST;

    //fw64_renderer_set_fog_positions(renderer, 0.4f, 0.8f);
    //fw64_renderer_set_fog_color(renderer, 85, 85, 85);

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
    renderer->shading_mode = FW64_SHADING_MODE_UNSET;
}

IVec2 fw64_renderer_get_viewport_size(fw64Renderer* renderer, fw64Camera* camera) {
    IVec2 viewport_size = {
        (int)(renderer->screen_size.x * camera->viewport.size.x),
        (int)(renderer->screen_size.y * camera->viewport.size.y)
    };

    return viewport_size;
}

void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64ClearFlags clear_flags) {
    renderer->primitive_mode = primitive_mode;
    fw64_texture_state_default(&renderer->active_texture);

    if (renderer->starting_new_frame) {
        renderer->display_list = &renderer->gfx_list[0];
    }
    
    renderer->display_list_start = renderer->display_list;

    gSPSegment(renderer->display_list++, 0, 0x0);
    fw64_renderer_init_rdp(renderer);
    fw64_renderer_init_rsp(renderer);

    if (clear_flags) {
        fw64_n64_renderer_clear_rect(renderer, 0, 0, renderer->screen_size.x, renderer->screen_size.y, renderer->clear_color, clear_flags);
    }

    //gSPSetLights2(renderer->display_list++, renderer->lights);
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererSwapFlags swap_flags) {
    gDPFullSync(renderer->display_list++);
    gSPEndDisplayList(renderer->display_list++);

    renderer->shading_mode = FW64_SHADING_MODE_UNSET;

    nuGfxTaskStart(renderer->display_list_start, 
        (s32)(renderer->display_list - renderer->display_list_start) * sizeof (Gfx), 
        (renderer->primitive_mode == FW64_PRIMITIVE_MODE_LINES) ? NU_GFX_UCODE_L3DEX2 : NU_GFX_UCODE_F3DEX, 
        (swap_flags & FW64_RENDERER_FLAG_SWAP) ? NU_SC_SWAPBUFFER : NU_SC_NOSWAPBUFFER);
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

/** Color = shade * primitive color  */ 
static void n64_renderer_configure_shading_mode_gouraud(fw64Renderer* renderer) {
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


    #define	FW64_N64_CC_SPRITE	TEXEL0, 0, SHADE, 0, TEXEL0, 0, SHADE, 0

    gDPSetRenderMode(renderer->display_list++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPClearGeometryMode(renderer->display_list++, G_ZBUFFER | G_LIGHTING | G_CULL_BOTH);
    gDPSetCombineMode(renderer->display_list++, FW64_N64_CC_SPRITE, FW64_N64_CC_SPRITE); // sprite
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);
    gDPPipeSync(renderer->display_list++);

    renderer->shading_mode = FW64_SHADING_MODE_SPRITE;
}

static void n64_renderer_configure_mesh_shading_mode(fw64Renderer* renderer, fw64Material* material) {
    switch(material->shading_mode) {
        case FW64_SHADING_MODE_VERTEX_COLOR:
            n64_renderer_configure_shading_mode_vertex_color(renderer);
            break;

        case FW64_SHADING_MODE_VERTEX_COLOR_TEXTURED:
            n64_renderer_configure_shading_mode_vertex_color_textured(renderer, material);
            break;

        case FW64_SHADING_MODE_UNLIT_TEXTURED:
            n64_renderer_configure_shading_mode_unlit_textured(renderer, material);
            break;

        case FW64_SHADING_MODE_DECAL_TEXTURE:
            n64_renderer_configure_shading_mode_decal_texture(renderer, material);
            break;
    }
}

static void n64_renderer_configure_lit_mesh_shading_mode(fw64Renderer* renderer, fw64Material* material) {
    switch(material->shading_mode) {
        case FW64_SHADING_MODE_GOURAUD:
            n64_renderer_configure_shading_mode_gouraud(renderer);
            break;

        case FW64_SHADING_MODE_GOURAUD_TEXTURED:
            n64_renderer_configure_shading_mode_gouraud_textured(renderer, material);
            break;

        default:
            break;
    }
}

static void fw64_n64_renderer_configure_lighting_info(fw64Renderer* renderer, LightingInfo* lighting_info, fw64Material* material) {
    // 11.7.3.6 Note on Material Properties
    // To obtain the correct light color in a particular situation, multiply the color of the material times the color of the light
    // for each light source and use the result as the light's color.
    int light_index = 0;
    for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
        if (!fw64_n64_lighting_info_light_is_enabled(lighting_info, i)) {
            continue;
        }

        light_index += 1;

        Light_t* light = &lighting_info->lights[i].l;
        uint32_t r = ((uint32_t)light->col[0] * (uint32_t)material->color.r) / 255;
        uint32_t g = ((uint32_t)light->col[1] * (uint32_t)material->color.g) / 255;
        uint32_t b = ((uint32_t)light->col[2] * (uint32_t)material->color.b) / 255;
        uint32_t light_val = (r << 24) | (g << 16) | (b << 8) | 255;
        //fw64ColorRGBA8 light_val = {r,g,b,255};

        //todo fix me
        gSPLightColor(renderer->display_list++, LIGHT_1, light_val);
    }
}

static void fw64_renderer_draw_lit_static_mesh(fw64Renderer* renderer, LightingInfo* lighting_info, fw64MeshInstance* mesh_instance) {
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&mesh_instance->n64_matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
    
    for (uint32_t i = 0 ; i < mesh_instance->mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh_instance->mesh->primitives + i;
        n64_renderer_configure_lit_mesh_shading_mode(renderer, primitive->material);
        fw64_n64_renderer_configure_lighting_info(renderer, lighting_info, primitive->material);
        
        gSPDisplayList(renderer->display_list++, primitive->display_list);
        gDPPipeSync(renderer->display_list++);
    }

    // note: pop is not necessary here...we are simply overwriting the MODELVIEW matrix see note above in set_camera
    //gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
}

static void fw64_renderer_draw_unlit_static_mesh(fw64Renderer* renderer, fw64MeshInstance* mesh_instance) {
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&mesh_instance->n64_matrix), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    
    for (uint32_t i = 0 ; i < mesh_instance->mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh_instance->mesh->primitives + i;
        
        n64_renderer_configure_mesh_shading_mode(renderer, primitive->material);
        gSPDisplayList(renderer->display_list++, primitive->display_list);
        gDPPipeSync(renderer->display_list++);
    }

    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
}

void fw64_renderer_draw_skinned_mesh(fw64Renderer* renderer, fw64SkinnedMeshInstance* instance) {
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&instance->mesh_instance.n64_matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
    
    fw64Mesh* mesh = instance->skinned_mesh->mesh;
    fw64AnimationController* controller = &instance->controller;

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
    gSPViewport(renderer->display_list++, &renderpass->n64_viewport);

    fw64Viewport* viewport = &renderpass->viewport;
    gDPSetScissor(renderer->display_list++, G_SC_NON_INTERLACE, 
    viewport->position.x, viewport->position.y, 
    viewport->position.x + viewport->size.x, viewport->position.y + viewport->size.y);

    if (renderpass->clear_flags) {
        fw64_n64_renderer_clear_viewport(renderer, &renderpass->viewport, renderpass->clear_color, renderpass->clear_flags);
    }

    renderer->enabled_features = renderpass->enabled_features;

    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG)) {
        gDPSetCycleType(renderer->display_list++, G_CYC_2CYCLE);
        gDPSetFogColor(renderer->display_list++, renderpass->fog_color.r, renderpass->fog_color.g, renderpass->fog_color.b, renderpass->fog_color.a);
        gSPFogPosition(renderer->display_list++, renderpass->fog_min, renderpass->fog_max);
        gSPSetGeometryMode(renderer->display_list++, G_FOG);
    }
    else {
        gDPSetCycleType(renderer->display_list++, G_CYC_1CYCLE);
        gSPClearGeometryMode(renderer->display_list++, G_FOG);
    }

    gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&renderpass->projection_matrix), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
    gSPPerspNormalize(renderer->display_list++, renderpass->persp_norm);

    if (!fw64_dynamic_vector_is_empty(&renderpass->render_queue.meshes[FW64_RENDER_QUEUE_UNLIT_STATIC]) || !fw64_dynamic_vector_is_empty(&renderpass->render_queue.meshes[FW64_RENDER_QUEUE_UNLIT_SKINNED])) {
        gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&renderpass->view_matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

        fw64DynamicVector* queue = &renderpass->render_queue.meshes[FW64_RENDER_QUEUE_UNLIT_STATIC];
        for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
            fw64MeshInstance* mesh_instance = *(fw64MeshInstance**)fw64_dynamic_vector_item(queue, i);
            fw64_renderer_draw_unlit_static_mesh(renderer, mesh_instance);
        }

        queue = &renderpass->render_queue.meshes[FW64_RENDER_QUEUE_UNLIT_SKINNED];
        for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
            fw64SkinnedMeshInstance* mesh_instance = *(fw64SkinnedMeshInstance**)fw64_dynamic_vector_item(queue, i);
            fw64_renderer_draw_skinned_mesh(renderer, mesh_instance);
        }
    }

    if (!fw64_dynamic_vector_is_empty(&renderpass->render_queue.meshes[FW64_RENDER_QUEUE_LIT_STATIC]) || !fw64_dynamic_vector_is_empty(&renderpass->render_queue.meshes[FW64_RENDER_QUEUE_LIT_SKINNED])) {
        LightingInfo* lighting_info = &renderpass->lighting_info;

        // When lighting is enabled, the camera view matrix should be present in the prjection matrix.  Slightly counter intuitive.
        // Refer to: 11.7.3.1 Important Note on Matrix Manipulation in the N64 Programming manual on ultra64.ca
        gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&renderpass->view_matrix), G_MTX_PROJECTION|G_MTX_MUL|G_MTX_NOPUSH);
        
        gSPNumLights(renderer->display_list++, NUMLIGHTS_1);
        // Setup light directions
        int light_index = 0;
        for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
            if (!fw64_n64_lighting_info_light_is_enabled(lighting_info, i)) {
                continue;
            }
            light_index += 1;
            gSPLight(renderer->display_list++, &(renderpass->lighting_info.lights[i]), 1);
        }
        gSPLight(renderer->display_list++, &(renderpass->lighting_info.ambient), 2);
        

        fw64DynamicVector* queue = &renderpass->render_queue.meshes[FW64_RENDER_QUEUE_LIT_STATIC];
        for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
            fw64MeshInstance* mesh_instance = *(fw64MeshInstance**)fw64_dynamic_vector_item(queue, i);
            fw64_renderer_draw_lit_static_mesh(renderer, lighting_info, mesh_instance);
        }
    }

    if (!fw64_dynamic_vector_is_empty(&renderpass->render_queue.sprite_batches)) {
        gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&renderpass->view_matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);
        n64_renderer_configure_shading_mode_sprite(renderer);

        for (size_t i = 0; i < fw64_dynamic_vector_size(&renderpass->render_queue.sprite_batches); i++){
            fw64SpriteBatch* sprite_batch = *((fw64SpriteBatch**)fw64_dynamic_vector_item(&renderpass->render_queue.sprite_batches, i));
            fw64_renderer_draw_texture_batch(renderer, &sprite_batch->texture_batch);
        }
    }
}

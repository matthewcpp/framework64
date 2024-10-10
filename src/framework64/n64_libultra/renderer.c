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

    renderer->clear_color = GPACK_RGBA5551(0, 0, 0, 1);
    memset(&renderer->empty_light, 0, sizeof(Light));

    renderer->enabled_features = N64_RENDERER_FEATURE_AA | N64_RENDERER_FEATURE_DEPTH_TEST;

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
        if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_DEPTH_TEST)) {
            gDPSetRenderMode(renderer->display_list++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
            gSPSetGeometryMode(renderer->display_list++, G_ZBUFFER );
        } else{
            gDPSetRenderMode(renderer->display_list++, G_RM_FOG_SHADE_A, G_RM_AA_OPA_SURF2);
            gSPClearGeometryMode(renderer->display_list++, G_ZBUFFER );
        }
    }
    else {
        if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_DEPTH_TEST)) {
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF);
            gSPSetGeometryMode(renderer->display_list++, G_ZBUFFER );
        } else {
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF);
            gSPClearGeometryMode(renderer->display_list++, G_ZBUFFER );
        }
    }
}

static inline void n64_renderer_set_render_mode_transparent(fw64Renderer* renderer) {
    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG)) {
        if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_DEPTH_TEST)) {
            gDPSetRenderMode(renderer->display_list++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
            gSPSetGeometryMode(renderer->display_list++, G_ZBUFFER );
        } else {
            gDPSetRenderMode(renderer->display_list++, G_RM_FOG_SHADE_A, G_RM_AA_XLU_SURF2);
            gSPClearGeometryMode(renderer->display_list++, G_ZBUFFER );
        }
    }
    else {
        if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_DEPTH_TEST)) {
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF);
            gSPSetGeometryMode(renderer->display_list++, G_ZBUFFER );
        } else {
            gDPSetRenderMode(renderer->display_list++, G_RM_AA_ZB_XLU_SURF, G_RM_AA_XLU_SURF);
            gSPClearGeometryMode(renderer->display_list++, G_ZBUFFER );
        }
    }
}

// static void n64_renderer_configure_shading_mode_decal_texture(fw64Renderer* renderer, fw64Material* material) {
//     renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, material->texture, material->texture_frame);
//     gDPSetPrimColor(renderer->display_list++, 0xFFFF, 0xFFFF, material->color.r, material->color.g, material->color.b, material->color.a);

//     if (renderer->shading_mode == FW64_SHADING_MODE_DECAL_TEXTURE)
//         return;

//     #define	FwW64_N64_CC_DECAL_TEX	TEXEL0, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0

//     gDPSetRenderMode(renderer->display_list++, G_RM_ZB_XLU_DECAL, G_RM_ZB_XLU_DECAL2);
//     gDPSetCombineMode(renderer->display_list++, FwW64_N64_CC_DECAL_TEX, FwW64_N64_CC_DECAL_TEX);
//     gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
//     gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

//     renderer->shading_mode = FW64_SHADING_MODE_DECAL_TEXTURE;
// }

static void fw64_n64_renderer_configure_mesh_lighting_info(fw64Renderer* renderer, LightingInfo* lighting_info, fw64Material* material) {
    // 11.7.3.6 Note on Material Properties
    // To obtain the correct light color in a particular situation, multiply the color of the material times the color of the light
    // for each light source and use the result as the light's color.
    int light_index = 0;
    for (int i = 0; i < FW64_RENDERER_MAX_LIGHT_COUNT; i++) {
        if (!fw64_n64_lighting_info_light_is_enabled(lighting_info, i)) {
            continue;
        }

        light_index += 1;

        Light_t* light = &lighting_info->lights[i].l;
        uint32_t r = ((uint32_t)light->col[0] * (uint32_t)material->color.r) / 255;
        uint32_t g = ((uint32_t)light->col[1] * (uint32_t)material->color.g) / 255;
        uint32_t b = ((uint32_t)light->col[2] * (uint32_t)material->color.b) / 255;
        uint32_t light_val = (r << 24) | (g << 16) | (b << 8) | 255;

        // Note: unfortunetly this is necessary due to the way gSPLightColor is setup.
        // Simply passing the index into the macro causes error.
        // Maybe this is something that can be fixed down the road.
        switch (light_index) {
            case LIGHT_1:
                gSPLightColor(renderer->display_list++, LIGHT_1, light_val);
                break;
            case LIGHT_2:
                gSPLightColor(renderer->display_list++, LIGHT_2, light_val);
                break;
            case LIGHT_3:
                gSPLightColor(renderer->display_list++, LIGHT_3, light_val);
                break;
            default:
                break;
        }
    }
}

static void fw64_renderer_draw_lit_primitive(fw64Renderer* renderer, LightingInfo* lighting_info, fw64StaticDrawInfo* draw_info) {
    fw64MeshInstance* mesh_instance = draw_info->instance;
    fw64Primitive* primitive = mesh_instance->mesh->primitives + draw_info->index;

    // when using lighting the view matrix is actually placed on the projection matrix stack, so we just load the model matrix
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&mesh_instance->n64_matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

    if (lighting_info->active_count > 0) {
        fw64_n64_renderer_configure_mesh_lighting_info(renderer, lighting_info, primitive->material);
    }
        
    gSPDisplayList(renderer->display_list++, primitive->display_list);
    gDPPipeSync(renderer->display_list++);

    // note: pop is not necessary here...we are simply overwriting the MODELVIEW matrix due
    // to the fact that the camera view matrix is included on the projection matrix stack.
    //gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
}

static void fw64_renderer_draw_lit_skinned_primitive(fw64Renderer* renderer, LightingInfo* lighting_info, fw64SkinnedDrawInfo* draw_info) {
    fw64SkinnedMeshInstance* skinned_mesh_instance = draw_info->instance;
    fw64AnimationController* controller = &skinned_mesh_instance->controller;
    fw64Primitive* primitive = skinned_mesh_instance->skinned_mesh->mesh->primitives + draw_info->index;

    // when using lighting the view matrix is actually placed on the projection matrix stack, so we just load the model matrix
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&skinned_mesh_instance->mesh_instance.n64_matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

    if (lighting_info->active_count > 0) {
        fw64_n64_renderer_configure_mesh_lighting_info(renderer, lighting_info, primitive->material);
    }

    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(controller->matrices + primitive->joint_index), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    
    gSPDisplayList(renderer->display_list++, primitive->display_list);
    gDPPipeSync(renderer->display_list++);

    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);

    // note: pop is not necessary here...we are simply overwriting the MODELVIEW matrix due
    // to the fact that the camera view matrix is included on the projection matrix stack.
    //gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
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

static void fw64_renderer_draw_unlit_primitive(fw64Renderer* renderer, fw64StaticDrawInfo* draw_info) {
    fw64MeshInstance* mesh_instance = draw_info->instance;
    fw64Primitive* primitive = mesh_instance->mesh->primitives + draw_info->index;

    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&mesh_instance->n64_matrix), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(renderer->display_list++, primitive->display_list);
    gDPPipeSync(renderer->display_list++);
    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
}

static void fw64_renderer_draw_unlit_skinned_primitive(fw64Renderer* renderer, fw64SkinnedDrawInfo* draw_info) {
    fw64SkinnedMeshInstance* skinned_mesh_instance = draw_info->instance;
    fw64AnimationController* controller = &skinned_mesh_instance->controller;
    fw64Primitive* primitive = skinned_mesh_instance->skinned_mesh->mesh->primitives + draw_info->index;

    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(&skinned_mesh_instance->mesh_instance.n64_matrix), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPMatrix(renderer->display_list++,OS_K0_TO_PHYSICAL(controller->matrices + primitive->joint_index), G_MTX_MODELVIEW|G_MTX_MUL|G_MTX_PUSH);
    gSPDisplayList(renderer->display_list++, primitive->display_list);
    gDPPipeSync(renderer->display_list++);

    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
    gSPPopMatrix(renderer->display_list++, G_MTX_MODELVIEW);
}

static void fw64_renderer_draw_unlit_queue_impl(fw64Renderer* renderer, fw64RenderPass* renderpass, fw64ShadingMode shading_mode) {
    gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&renderpass->view_matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

    gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);
    gSPSetGeometryMode(renderer->display_list++, G_CULL_BACK);
    gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF );

    fw64RenderQueueBucket* render_queue_bucket = fw64_render_queue_get_bucket(&renderpass->render_queue, shading_mode);
    fw64DynamicVector* queue = &render_queue_bucket->static_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64StaticDrawInfo* draw_info = (fw64StaticDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64_renderer_draw_unlit_primitive(renderer, draw_info);
    }

    queue = &render_queue_bucket->skinned_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64SkinnedDrawInfo* draw_info = (fw64SkinnedDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64_renderer_draw_unlit_skinned_primitive(renderer, draw_info);
    }
}

static void fw64_renderer_draw_unlit_queue(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    n64_renderer_set_render_mode_opaque(renderer);
    fw64_renderer_draw_unlit_queue_impl(renderer, renderpass, FW64_SHADING_MODE_UNLIT);
}

static void fw64_renderer_draw_unlit_textured_queue_impl(fw64Renderer* renderer, fw64RenderPass* renderpass, fw64ShadingMode shading_mode) {
    gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&renderpass->view_matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

    gSPSetGeometryMode(renderer->display_list++, G_CULL_BACK);
    gSPClearGeometryMode(renderer->display_list++, G_LIGHTING);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);

    fw64RenderQueueBucket* render_queue_bucket = fw64_render_queue_get_bucket(&renderpass->render_queue, shading_mode);
    fw64DynamicVector* queue = &render_queue_bucket->static_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64StaticDrawInfo* draw_info = (fw64StaticDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64Material* material = fw64_mesh_get_material_for_primitive(draw_info->instance->mesh, draw_info->index);
        renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, material->texture, material->texture_frame);
        fw64_renderer_draw_unlit_primitive(renderer, draw_info);
    }

    queue = &render_queue_bucket->skinned_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64SkinnedDrawInfo* draw_info = (fw64SkinnedDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64Material* material = fw64_mesh_get_material_for_primitive(draw_info->instance->skinned_mesh->mesh, draw_info->index);
        renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, material->texture, material->texture_frame);
        fw64_renderer_draw_unlit_skinned_primitive(renderer, draw_info);
    }
}

static void fw64_renderer_draw_unlit_textured_queue(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    n64_renderer_set_render_mode_opaque(renderer);
    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG)){
        gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGB, G_CC_MODULATERGB2);
    }
    else {
        gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGB, G_CC_MODULATERGB);
    }

    fw64_renderer_draw_unlit_textured_queue_impl(renderer, renderpass, FW64_SHADING_MODE_UNLIT_TEXTURED);
}

static void fw64_renderer_draw_unlit_transparent_queue(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    n64_renderer_set_render_mode_transparent(renderer);
    if (GET_RENDERER_FEATURE(renderer, N64_RENDERER_FEATURE_FOG)){
        gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGBA, G_CC_MODULATERGBA2);
    }
    else {
        gDPSetCombineMode(renderer->display_list++, G_CC_MODULATERGBA, G_CC_MODULATERGBA);
    }
    fw64_renderer_draw_unlit_textured_queue_impl(renderer, renderpass, FW64_SHADING_MODE_UNLIT_TRANSPARENT_TEXTURED);
}

static void fw64_renderer_configure_lighting(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    LightingInfo* lighting_info = &renderpass->lighting_info;

    // When lighting is enabled, the camera view matrix should be present in the prjection matrix.  Slightly counter intuitive.
    // Refer to: 11.7.3.1 Important Note on Matrix Manipulation in the N64 Programming manual on ultra64.ca
    gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&renderpass->view_matrix), G_MTX_PROJECTION|G_MTX_MUL|G_MTX_NOPUSH);
    
    if (lighting_info->active_count > 0) {
        gSPNumLights(renderer->display_list++, lighting_info->active_count);

        // Setup light directions
        int light_index = 0;
        for (int i = 0; i < FW64_RENDERER_MAX_LIGHT_COUNT; i++) {
            if (!fw64_n64_lighting_info_light_is_enabled(lighting_info, i)) {
                continue;
            }
            light_index += 1;
            gSPLight(renderer->display_list++, &(renderpass->lighting_info.lights[i]), light_index);
        }

        // set the ambient light
        light_index += 1;
        gSPLight(renderer->display_list++, &(renderpass->lighting_info.ambient), light_index);
    }
    else {
        // note, in the case of 0 lights then we recreate set setup for gSPSetLights0, refer to gbi.h
        gSPNumLights(renderer->display_list++, 1);
        gSPLight(renderer->display_list++, &(renderer->empty_light), 1);
        gSPLight(renderer->display_list++, &(renderpass->lighting_info.ambient), 2);
    }
}

static void fw64_renderer_render_lit_queue(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    n64_renderer_set_render_mode_opaque(renderer);
    // what about fog?
    gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);

    gSPSetGeometryMode(renderer->display_list++, G_LIGHTING | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);
    gSPTexture(renderer->display_list++, 0, 0, 0, 0, G_OFF );

    fw64RenderQueueBucket* render_queue_bucket = fw64_render_queue_get_bucket(&renderpass->render_queue, FW64_SHADING_MODE_LIT);
    fw64DynamicVector* queue = &render_queue_bucket->static_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64StaticDrawInfo* draw_info = (fw64StaticDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64_renderer_draw_lit_primitive(renderer, &renderpass->lighting_info, draw_info);
    }

    queue = &render_queue_bucket->skinned_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64SkinnedDrawInfo* draw_info = (fw64SkinnedDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64_renderer_draw_lit_skinned_primitive(renderer, &renderpass->lighting_info, draw_info);
    }
}

static void fw64_renderer_render_lit_textured_queue(fw64Renderer* renderer, fw64RenderPass* renderpass) {
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

    fw64RenderQueueBucket* render_queue_bucket = fw64_render_queue_get_bucket(&renderpass->render_queue, FW64_SHADING_MODE_LIT_TEXTURED);
    fw64DynamicVector* queue = &render_queue_bucket->static_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64StaticDrawInfo* draw_info = (fw64StaticDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64Material* material = fw64_mesh_get_material_for_primitive(draw_info->instance->mesh, draw_info->index);
        renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, material->texture, material->texture_frame);
        fw64_renderer_draw_lit_primitive(renderer, &renderpass->lighting_info, draw_info);
    }

    queue = &render_queue_bucket->skinned_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64SkinnedDrawInfo* draw_info = (fw64SkinnedDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64Material* material = fw64_mesh_get_material_for_primitive(draw_info->instance->skinned_mesh->mesh, draw_info->index);
        renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, material->texture, material->texture_frame);
        fw64_renderer_draw_lit_skinned_primitive(renderer, &renderpass->lighting_info, draw_info);
    }
}

static void fw64_renderer_render_sprite_batches(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&renderpass->view_matrix), G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

    gDPSetRenderMode(renderer->display_list++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPClearGeometryMode(renderer->display_list++, G_LIGHTING | G_CULL_BOTH);
    gDPSetCombineMode(renderer->display_list++, G_CC_MODULATEIA, G_CC_MODULATEIA);
    gSPTexture(renderer->display_list++, 0x8000, 0x8000, 0, 0, G_ON );
    gDPSetTexturePersp(renderer->display_list++, G_TP_PERSP);
    gDPPipeSync(renderer->display_list++);

    for (size_t i = 0; i < fw64_dynamic_vector_size(&renderpass->render_queue.sprite_batches); i++){
        fw64SpriteBatch* sprite_batch = *((fw64SpriteBatch**)fw64_dynamic_vector_item(&renderpass->render_queue.sprite_batches, i));
        fw64N64TextureBatch* texture_batch = &sprite_batch->texture_batch;

        for (size_t l = 0; l < texture_batch->layer_count; l++) {
            fw64TextureBatchLayer* layer = texture_batch->layers + l;

            for (uint32_t i = 0; i < layer->batch_count; i++) {
                fw64N64BatchedTexture* batched_texure = layer->batches[i];
                renderer->display_list = fw64_n64_load_texture(&renderer->active_texture, renderer->display_list, batched_texure->id.tex_info.texture, batched_texure->id.tex_info.index);
                gDPPipeSync(renderer->display_list++);
                fw64N64BatchedVertexChunk* chunk = batched_texure->first_chunk;

                while (chunk) {
                    gSPDisplayList(renderer->display_list++, chunk->display_list);
                    gDPPipeSync(renderer->display_list++);
                    chunk = chunk->next;
                }
            }
        }
    }
}

static void fw64_renderer_setup_projection_matrix(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    gSPMatrix(renderer->display_list++, OS_K0_TO_PHYSICAL(&renderpass->projection_matrix), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
    gSPPerspNormalize(renderer->display_list++, renderpass->persp_norm);
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

    fw64_renderer_setup_projection_matrix(renderer, renderpass);

    if (fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_UNLIT)) {
        fw64_renderer_draw_unlit_queue(renderer, renderpass);
    }

    if (fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_UNLIT_TEXTURED)) {
        fw64_renderer_draw_unlit_textured_queue(renderer, renderpass);
    }

    int has_lit_items = fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_LIT) ||
                        fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_LIT_TEXTURED);

    if (has_lit_items) {
        fw64_renderer_configure_lighting(renderer, renderpass);

        if (fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_LIT)) {
            fw64_renderer_render_lit_queue(renderer, renderpass);
        }
        
        if (fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_LIT_TEXTURED)) {
            fw64_renderer_render_lit_textured_queue(renderer, renderpass);
        }
    }

    // if we rendered lit items and are now back to rendering unlit items then we need to reset the projection matrix
    // since the lighting calculations will have placed the view matrix in it.
    if (has_lit_items) {
        fw64_renderer_setup_projection_matrix(renderer, renderpass);
    }

    if (fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_UNLIT_TRANSPARENT_TEXTURED)) {
        fw64_renderer_draw_unlit_transparent_queue(renderer, renderpass);
    }

    if (fw64_render_queue_has_sprite_batches(&renderpass->render_queue)) {
        fw64_renderer_render_sprite_batches(renderer, renderpass);
    }
}

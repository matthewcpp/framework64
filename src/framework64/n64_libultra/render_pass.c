#include "framework64/n64_libultra/render_pass.h"

#include "framework64/math.h"

static void update_n64_viewport(const fw64Viewport* viewport, Vp* n64_viewport);

fw64RenderPass* fw64_renderpass_create(fw64Display* display, fw64Allocator* allocator) {
    fw64RenderPass* renderpass = fw64_allocator_memalign(allocator, 8, sizeof(fw64RenderPass));
    renderpass->allocator = allocator;
    fw64_render_queue_init(&renderpass->render_queue, allocator);

    ivec2_set(&renderpass->viewport.position, 0, 0);
    renderpass->viewport.size = fw64_display_get_size(display);
    update_n64_viewport(&renderpass->viewport, &renderpass->n64_viewport);

    guMtxIdent(&renderpass->projection_matrix);
    guMtxIdent(&renderpass->view_matrix);

    renderpass->persp_norm = FW64_N64_LIBULTRA_DEFAULT_PERSPNORM;
    renderpass->clear_flags = FW64_CLEAR_FLAG_DEFAULT;
    renderpass->clear_color = GPACK_RGBA5551(0, 0, 0, 1);

    renderpass->enabled_features = N64_RENDERER_FEATURE_AA | N64_RENDERER_FEATURE_DEPTH_TEST;
    renderpass->primitive_mode = FW64_PRIMITIVE_MODE_TRIANGLES;
    fw64ColorRGBA8 light_color = {255, 255, 255, 255};
    fw64ColorRGBA8 ambient_light_color = {25, 25, 25, 255};
    Vec3 light_dir = {0.57735f, -0.57735f, 0.57735};

    renderpass->lighting_info.active_count = 0;
    for (int i = 0; i < FW64_RENDERER_MAX_LIGHT_COUNT; i++) {
        fw64_renderpass_set_light_direction(renderpass, i, &light_dir);
        fw64_renderpass_set_light_color(renderpass, i, light_color);
        fw64_n64_lighting_info_set_enabled_flag(&renderpass->lighting_info, i, 0);
        fw64_renderpass_set_light_enabled(renderpass, i , i == 0);
    }

    fw64_renderpass_set_lighting_ambient_color(renderpass, ambient_light_color);

    return renderpass;
}

void fw64_renderpass_delete(fw64RenderPass* renderpass) {
    fw64_render_queue_uninit(&renderpass->render_queue);
    fw64_allocator_free(renderpass->allocator, renderpass);
}

void fw64_renderpass_begin(fw64RenderPass* renderpass) {
    fw64_render_queue_clear(&renderpass->render_queue);
}

void fw64_renderpass_end(fw64RenderPass* renderpass) {
    (void)renderpass;
}

void fw64_renderpass_set_primitive_mode(fw64RenderPass* renderpass, fw64PrimitiveMode primitive_mode) {
    renderpass->primitive_mode = primitive_mode;
}

void fw64_renderpass_set_view_matrix(fw64RenderPass* pass, float* view) {
    guMtxF2L((float (*)[4])view, &pass->view_matrix);
}

void fw64_renderpass_set_projection_matrix(fw64RenderPass* pass, float* projection, uint16_t* persp_norm_ptr) {
    guMtxF2L((float (*)[4])projection, &pass->projection_matrix);

    if (persp_norm_ptr) {
        pass->persp_norm = *persp_norm_ptr;
    } else {
        pass->persp_norm = FW64_N64_LIBULTRA_DEFAULT_PERSPNORM;
    }
}

void update_n64_viewport(const fw64Viewport* viewport, Vp* n64_viewport) {
    n64_viewport->vp.vscale[0] = viewport->size.x * 2;
    n64_viewport->vp.vscale[1] = viewport->size.y * 2;
    n64_viewport->vp.vscale[2] = G_MAXZ / 2;
    n64_viewport->vp.vscale[3] = 0;

    n64_viewport->vp.vtrans[0] = ((viewport->position.x * 2) + viewport->size.x) * 2;
    n64_viewport->vp.vtrans[1] = ((viewport->position.y * 2) + viewport->size.y) * 2;
    n64_viewport->vp.vtrans[2] = G_MAXZ / 2;
    n64_viewport->vp.vtrans[3] = 0;
}

void fw64_renderpass_set_viewport(fw64RenderPass* renderpass, const fw64Viewport* viewport) {
    renderpass->viewport = *viewport;
    update_n64_viewport(viewport, &renderpass->n64_viewport);
}

const fw64Viewport* fw64_renderpass_get_viewport(fw64RenderPass* renderpass) {
    return &renderpass->viewport;
}

void fw64_renderpass_set_camera(fw64RenderPass* pass, fw64Camera* camera) {
    pass->viewport = camera->viewport;
    pass->projection_matrix = camera->projection;
    pass->view_matrix = camera->view;
    pass->persp_norm = camera->_persp_norm;

    update_n64_viewport(&pass->viewport, &pass->n64_viewport);
}

void fw64_renderpass_set_clear_flags(fw64RenderPass* pass, fw64ClearFlags flags) {
    pass->clear_flags = flags;
}

void fw64_renderpass_set_clear_color(fw64RenderPass* pass, uint8_t r, uint8_t g, uint8_t b) {
    pass->clear_color = GPACK_RGBA5551(r, g, b, 1);
}

void fw64_renderpass_draw_static_mesh(fw64RenderPass* renderpass, fw64MeshInstance* mesh_instance) {
    fw64_render_queue_enqueue_static_mesh(&renderpass->render_queue, mesh_instance);
}

void fw64_renderpass_draw_skinned_mesh(fw64RenderPass* pass, fw64SkinnedMeshInstance* instance) {
    fw64_render_queue_enqueue_skinned_mesh(&pass->render_queue, instance);
}

void fw64_renderpass_draw_sprite_batch(fw64RenderPass* renderpass, fw64SpriteBatch* sprite_batch) {
    fw64_render_queue_enqueue_sprite_batch(&renderpass->render_queue, sprite_batch);
}

static void fw64_n64_renderpass_toggle_feature(fw64RenderPass* renderpass, fw64N64RendererFeature feature, int enabled) {
    if (enabled) {
        SET_RENDERER_FEATURE(renderpass, feature);
    } else {
        UNSET_RENDERER_FEATURE(renderpass, feature);
    }
}

void fw64_renderpass_set_depth_testing_enabled(fw64RenderPass* renderpass, int enabled) {
    fw64_n64_renderpass_toggle_feature(renderpass, N64_RENDERER_FEATURE_DEPTH_TEST, enabled);
}

void fw64_renderpass_set_anti_aliasing_enabled(fw64RenderPass* renderpass, int enabled) {
    fw64_n64_renderpass_toggle_feature(renderpass, N64_RENDERER_FEATURE_AA, enabled);
}

void fw64_renderpass_set_fog_enabled(fw64RenderPass* renderpass, int enabled) {
    fw64_n64_renderpass_toggle_feature(renderpass, N64_RENDERER_FEATURE_FOG, enabled);
}

/**
 * The N64 Fog Algorithm is not totally clear.
 * This implementation attempts to provide a reasonable approximation of how I think it should work.
 * Note that a crash has been observed if fog_min == fog_max
 */
void fw64_renderpass_set_fog_positions(fw64RenderPass* renderpass, float fog_min, float fog_max) {
    renderpass->fog_min = (s32)fw64_clamp(900.0f + (fog_min * 100.0f), 0.0f, 1000.0f);
    renderpass->fog_max = (s32)fw64_clamp(900.0f + (fog_max * 100.0f), 0.0f, 1000.0f);

    if (renderpass->fog_min == renderpass->fog_max) {
        renderpass->fog_min = renderpass->fog_max - 8;
    }
}

void fw64_renderpass_set_fog_color(fw64RenderPass* renderpass, uint8_t r, uint8_t g, uint8_t b) {
    fw64_color_rgba8_set(&renderpass->fog_color, r, g, b, 255);
}

void fw64_renderpass_set_light_enabled(fw64RenderPass* renderpass, int index, int enabled) {
    // disable light
    if (fw64_n64_lighting_info_light_is_enabled(&renderpass->lighting_info, index) && !enabled) {
        renderpass->lighting_info.active_count -= 1;
        fw64_n64_lighting_info_set_enabled_flag(&renderpass->lighting_info, index, 0);
    } // enable light
    else if (!fw64_n64_lighting_info_light_is_enabled(&renderpass->lighting_info, index) && enabled) {
        renderpass->lighting_info.active_count += 1;
        fw64_n64_lighting_info_set_enabled_flag(&renderpass->lighting_info, index, 1);
    }
}

#define LIGHT_FACTOR 80.0f
// [11.7.3.2 Light Structure Definition ]
// The convention is that the light direction points toward the light. 
// This means the light direction indicates the direction TO the light and NOT the direction that the light is shining.
void fw64_renderpass_set_light_direction(fw64RenderPass* renderpass, int index, Vec3* direction) {
    Light_t* light = &renderpass->lighting_info.lights[index].l;

    light->dir[0] = (int8_t)(-direction->x * LIGHT_FACTOR);
    light->dir[1] = (int8_t)(-direction->y * LIGHT_FACTOR);
    light->dir[2] = (int8_t)(-direction->z * LIGHT_FACTOR);
}

void fw64_renderpass_set_light_color(fw64RenderPass* renderpass, int index, fw64ColorRGBA8 color) {
    Light_t* light = &renderpass->lighting_info.lights[index].l;

    light->col[0] = color.r;
    light->col[1] = color.g;
    light->col[2] = color.b;

    light->colc[0] = color.r;
    light->colc[1] = color.g;
    light->colc[2] = color.b;
}

void fw64_renderpass_set_lighting_ambient_color(fw64RenderPass* renderpass, fw64ColorRGBA8 color) {
    Ambient_t* ambient = &renderpass->lighting_info.ambient.l;

    ambient->col[0] = color.r;
    ambient->col[1] = color.g;
    ambient->col[2] = color.b;

    ambient->colc[0] = color.r;
    ambient->colc[1] = color.g;
    ambient->colc[2] = color.b;
}

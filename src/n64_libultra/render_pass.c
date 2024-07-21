#include "framework64/n64_libultra/render_pass.h"

#include "framework64/math.h"

fw64RenderPass* fw64_renderpass_create(fw64Display* display, fw64Allocator* allocator) {
    fw64RenderPass* renderpass = allocator->memalign(allocator, 8, sizeof(fw64RenderPass));
    renderpass->allocator = allocator;
    fw64_n64_render_queue_init(&renderpass->render_queue, allocator);

    ivec2_set(&renderpass->viewport.position, 0, 0);
    renderpass->viewport.size = fw64_display_get_size(display);
    update_n64_viewport(&renderpass->viewport, &renderpass->n64_viewport);

    guMtxIdent(&renderpass->projection_matrix);
    guMtxIdent(&renderpass->view_matrix);

    renderpass->persp_norm = FW64_N64_LIBULTRA_DEFAULT_PERSPNORM;
    renderpass->clear_flags = FW64_CLEAR_FLAG_NONE;
    renderpass->clear_color = GPACK_RGBA5551(0, 0, 0, 1);

    renderpass->enabled_features = N64_RENDERER_FEATURE_AA | N64_RENDERER_FEATURE_DEPTH_TEST;

    return renderpass;
}

void fw64_renderpass_delete(fw64RenderPass* renderpass) {
    fw64_n64_render_queue_uninit(&renderpass->render_queue);
    renderpass->allocator->free(renderpass->allocator, renderpass);
}

void fw64_renderpass_begin(fw64RenderPass* renderpass) {
    fw64_n64_render_queue_clear(&renderpass->render_queue);
}

void fw64_renderpass_end(fw64RenderPass* renderpass) {
    (void)renderpass;
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

void update_n64_viewport(fw64Viewport* viewport, Vp* n64_viewport) {
    n64_viewport->vp.vscale[0] = viewport->size.x * 2;
    n64_viewport->vp.vscale[1] = viewport->size.y * 2;
    n64_viewport->vp.vscale[2] = G_MAXZ / 2;
    n64_viewport->vp.vscale[3] = 0;

    n64_viewport->vp.vtrans[0] = ((viewport->position.x * 2) + viewport->size.x) * 2;
    n64_viewport->vp.vtrans[1] = ((viewport->position.y * 2) + viewport->size.y) * 2;
    n64_viewport->vp.vtrans[2] = G_MAXZ / 2;
    n64_viewport->vp.vtrans[3] = 0;
}

void fw64_renderpass_set_viewport(fw64RenderPass* renderpass, fw64Viewport* viewport) {
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
}

void fw64_renderpass_set_clear_flags(fw64RenderPass* pass, fw64ClearFlags flags) {
    pass->clear_flags = flags;
}

void fw64_renderpass_set_clear_color(fw64RenderPass* pass, uint8_t r, uint8_t g, uint8_t b) {
    pass->clear_color = GPACK_RGBA5551(r, g, b, 1);
}

void fw64_renderpass_draw_static_mesh(fw64RenderPass* renderpass, fw64MeshInstance* mesh_instance) {
    fw64_n64_render_queue_enqueue_static_mesh(&renderpass->render_queue, mesh_instance);
}

void fw64_renderpass_draw_skinned_mesh(fw64RenderPass* pass, fw64SkinnedMeshInstance* instance) {
    fw64_n64_render_queue_enqueue_skinned_mesh(&pass->render_queue, instance);
}

void fw64_renderpass_draw_sprite_batch(fw64RenderPass* renderpass, fw64SpriteBatch* sprite_batch) {
    fw64_n64_render_queue_enqueue_sprite_batch(&renderpass->render_queue, sprite_batch);
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

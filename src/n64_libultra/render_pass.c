#include "framework64/n64/render_pass.h"

fw64RenderPass* fw64_renderpass_create(fw64Display* display, fw64Allocator* allocator) {
    fw64RenderPass* renderpass = allocator->malloc(allocator, sizeof(fw64RenderPass));
    renderpass->allocator = allocator;
    fw64_n64_render_queue_init(&renderpass->render_queue, allocator);

    IVec2 viewport_pos = {0,0};
    IVec2 viewport_size = fw64_display_get_size(display);
    fw64_viewport_set(&renderpass->viewport, &viewport_pos, &viewport_size);

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

void fw64_renderpass_set_viewport(fw64RenderPass* pass, fw64Viewport* viewport) {
    pass->viewport = *viewport;
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

void fw64_renderpass_draw_static_mesh(fw64RenderPass* renderpass, fw64Mesh* mesh, fw64Transform* transform) {
    fw64_n64_render_queue_enqueue_static_mesh(&renderpass->render_queue, mesh, transform);
}

void fw64_renderpass_draw_animated_mesh(fw64RenderPass* renderpass, fw64Mesh* mesh, fw64AnimationController* controller, fw64Transform* transform) {

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
#include "libdragon_render_pass.h"

#include "framework64/matrix.h"

#include <string.h>

fw64RenderPass* fw64_renderpass_create(fw64Display* display, fw64Allocator* allocator) {
    fw64RenderPass* renderpass = fw64_allocator_malloc(allocator, sizeof(fw64RenderPass));
    renderpass->allocator = allocator;
    renderpass->render_features = FW64_LIBDRAGON_RENDERPASS_FEATURE_DEPTH_TESTING;

    IVec2 viewport_pos = {0,0};
    IVec2 viewport_size = fw64_display_get_size(display);
    fw64_viewport_set(&renderpass->viewport, &viewport_pos, &viewport_size);

    fw64_render_queue_init(&renderpass->render_queue, allocator);
    matrix_set_identity(renderpass->projection_matrix);
    matrix_set_identity(renderpass->view_matrix);

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

void fw64_renderpass_set_depth_testing_enabled(fw64RenderPass* renderpass, int enabled) {
    if (enabled) {
        renderpass->render_features |= FW64_LIBDRAGON_RENDERPASS_FEATURE_DEPTH_TESTING;
    } else {
        renderpass->render_features &= ~FW64_LIBDRAGON_RENDERPASS_FEATURE_DEPTH_TESTING;
    }
}

const fw64Viewport* fw64_renderpass_get_viewport(fw64RenderPass* renderpass) {
    return &renderpass->viewport;
}

void fw64_renderpass_set_view_matrix(fw64RenderPass* renderpass, float* view) {
    memcpy(renderpass->view_matrix, view, sizeof(float) * 16);
}

void fw64_renderpass_set_projection_matrix(fw64RenderPass* renderpass, float* projection, uint16_t* persp_norm_ptr) {
    (void)persp_norm_ptr;
    memcpy(renderpass->projection_matrix, projection, sizeof(float) * 16);
}

void fw64_renderpass_draw_sprite_batch(fw64RenderPass* renderpass, fw64SpriteBatch* spritebatch) {
    fw64_render_queue_enqueue_sprite_batch(&renderpass->render_queue, spritebatch);
}

void fw64_renderpass_draw_static_mesh(fw64RenderPass* renderpass, fw64MeshInstance* mesh_instance){
    fw64_render_queue_enqueue_static_mesh(&renderpass->render_queue, mesh_instance);
}

void fw64_renderpass_draw_skinned_mesh(fw64RenderPass* renderpass, fw64SkinnedMeshInstance* skinned_mesh_instance) {
    fw64_render_queue_enqueue_skinned_mesh(&renderpass->render_queue, skinned_mesh_instance);
}

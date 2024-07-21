#include "framework64/desktop/render_pass.hpp"

#include "framework64/matrix.h"

#include <cassert>

fw64RenderPass::fw64RenderPass(fw64Display* display, fw64Allocator* allocator) 
    : allocator(allocator) 
{
    IVec2 viewport_pos = {0,0};
    auto viewport_size = fw64_display_get_size(display);
    fw64_viewport_set(&viewport, &viewport_pos, &viewport_size);

    matrix_set_identity(projection_matrix.data());
    matrix_set_identity(view_matrix.data());
}

void fw64RenderPass::begin() {
    render_queue.clear();
}

void fw64RenderPass::end() {

}

// C-API

fw64RenderPass* fw64_renderpass_create(fw64Display* display, fw64Allocator* allocator) {
    assert(allocator);

    return new fw64RenderPass(display, allocator);
}

void fw64_renderpass_delete(fw64RenderPass* renderpass) {
    delete renderpass;
}

void fw64_renderpass_begin(fw64RenderPass* renderpass) {
    renderpass->begin();
}

void fw64_renderpass_end(fw64RenderPass* renderpass) {
    renderpass->end();
}

void fw64_renderpass_set_view_matrix(fw64RenderPass* pass, float* view) {
    pass->setViewMatrix(view);
}

void fw64_renderpass_set_projection_matrix(fw64RenderPass* pass, float* projection, uint16_t*) {
    pass->setProjectionMatrix(projection);
}

void fw64_renderpass_set_viewport(fw64RenderPass* pass, fw64Viewport* viewport) {
    pass->viewport = *viewport;
}

const fw64Viewport* fw64_renderpass_get_viewport(fw64RenderPass* renderpass) {
    return &renderpass->viewport;
}

void fw64_renderpass_set_camera(fw64RenderPass* pass, fw64Camera* camera) {
    pass->setViewport(camera->viewport);
    pass->setViewMatrix(camera->view.m);
    pass->setProjectionMatrix(camera->projection.m);

    pass->camera_near = camera->near;
    pass->camera_far = camera->far;
}

void fw64_renderpass_set_clear_flags(fw64RenderPass* pass, fw64ClearFlags flags){
    pass->clear_flags = flags;
}

void fw64_renderpass_set_clear_color(fw64RenderPass* pass, uint8_t r, uint8_t g, uint8_t b) {
    pass->clear_color[0] = static_cast<float>(r) / 255.0f;
    pass->clear_color[1] = static_cast<float>(g) / 255.0f;
    pass->clear_color[2] = static_cast<float>(b) / 255.0f;
}

void fw64_renderpass_draw_sprite_batch(fw64RenderPass* renderpass, fw64SpriteBatch* sprite_batch) {
    renderpass->render_queue.sprite_batches.emplace_back(sprite_batch);
}

void fw64_renderpass_draw_static_mesh(fw64RenderPass* renderpass, fw64MeshInstance* mesh_instance) {
    renderpass->render_queue.mesh_instances.emplace_back(mesh_instance);
}

void fw64_renderpass_draw_skinned_mesh(fw64RenderPass* renderpass, fw64SkinnedMeshInstance* skinned_mesh_instance) {
    renderpass->render_queue.skinned_mesh_instances.emplace_back(skinned_mesh_instance);
}

void fw64_renderpass_set_depth_testing_enabled(fw64RenderPass* renderpass, int enabled) {
    renderpass->depth_testing_enabled = static_cast<bool>(enabled);
}

void fw64_renderpass_set_fog_enabled(fw64RenderPass* renderpass, int enabled) {
    renderpass->fog_enabled = static_cast<bool>(enabled);
}

void fw64_renderpass_set_fog_positions(fw64RenderPass* renderpass, float fog_min, float fog_max) {
    assert( fog_min >= 0.0f && fog_min <= 1.0f &&
        fog_max >= 0.0f && fog_max <= 1.0f &&
        fog_max >= fog_min);

    renderpass->fog_begin = fog_min;
    renderpass->fog_end = fog_max;
}

void fw64_renderpass_set_fog_color(fw64RenderPass* renderpass, uint8_t r, uint8_t g, uint8_t b) {
    renderpass->fog_color[0] = r / 255.0f;
    renderpass->fog_color[1] = g / 255.0f;
    renderpass->fog_color[2] = b / 255.0f;
}

#include "libdragon_render_pass.h"

fw64RenderPass* fw64_renderpass_create(fw64Display* display, fw64Allocator* allocator) {
    fw64RenderPass* renderpass = fw64_allocator_malloc(allocator, sizeof(fw64RenderPass));
    renderpass->allocator = allocator;

    fw64_render_queue_init(&renderpass->render_queue, allocator);

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

void fw64_renderpass_draw_sprite_batch(fw64RenderPass* renderpass, fw64SpriteBatch* spritebatch) {
    fw64_render_queue_enqueue_sprite_batch(&renderpass->render_queue, spritebatch);
}
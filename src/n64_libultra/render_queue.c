#include "framework64/n64/render_queue.h"

#include <string.h>

void fw64_n64_render_queue_init(fw64N64RenderQueue* render_queue, fw64Allocator* allocator) {
    fw64_dynamic_vector_init(&render_queue->sprite_batches, sizeof(fw64SpriteBatch*), allocator);
    fw64_dynamic_vector_init(&render_queue->static_meshes, sizeof(fw64N64MeshInstance), allocator);
}

void fw64_n64_render_queue_uninit(fw64N64RenderQueue* render_queue) {
    fw64_dynamic_vector_uninit(&render_queue->sprite_batches);
    fw64_dynamic_vector_uninit(&render_queue->static_meshes);
}

void fw64_n64_render_queue_clear(fw64N64RenderQueue* render_queue) {
    fw64_dynamic_vector_clear(&render_queue->sprite_batches);
    fw64_dynamic_vector_clear(&render_queue->static_meshes);
}

void fw64_n64_render_queue_enqueue_sprite_batch(fw64N64RenderQueue* render_queue, fw64SpriteBatch* sprite_batch) {
    fw64_dynamic_vector_push_back(&render_queue->sprite_batches, &sprite_batch);
}

void fw64_n64_render_queue_enqueue_static_mesh(fw64N64RenderQueue* render_queue,  fw64Mesh* mesh, fw64Transform* transform) {
    fw64N64MeshInstance* mesh_instance = (fw64N64MeshInstance*)fw64_dynamic_vector_emplace_back(&render_queue->static_meshes);
    mesh_instance->mesh = mesh;
    mesh_instance->transform = transform;
}
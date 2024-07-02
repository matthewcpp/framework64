#include "framework64/n64_libultra/render_queue.h"

#include <string.h>

void fw64_n64_render_queue_init(fw64N64RenderQueue* render_queue, fw64Allocator* allocator) {
    fw64_dynamic_vector_init(&render_queue->sprite_batches, sizeof(fw64SpriteBatch*), allocator);
    fw64_dynamic_vector_init(&render_queue->static_meshes, sizeof(fw64MeshInstance*), allocator);
    fw64_dynamic_vector_init(&render_queue->skinned_meshes, sizeof(fw64SkinnedMeshInstance*), allocator);
}

void fw64_n64_render_queue_uninit(fw64N64RenderQueue* render_queue) {
    fw64_dynamic_vector_uninit(&render_queue->sprite_batches);
    fw64_dynamic_vector_uninit(&render_queue->static_meshes);
    fw64_dynamic_vector_uninit(&render_queue->skinned_meshes);
}

void fw64_n64_render_queue_clear(fw64N64RenderQueue* render_queue) {
    fw64_dynamic_vector_clear(&render_queue->sprite_batches);
    fw64_dynamic_vector_clear(&render_queue->static_meshes);
    fw64_dynamic_vector_clear(&render_queue->skinned_meshes);
}

void fw64_n64_render_queue_enqueue_sprite_batch(fw64N64RenderQueue* render_queue, fw64SpriteBatch* sprite_batch) {
    fw64_dynamic_vector_push_back(&render_queue->sprite_batches, &sprite_batch);
}

void fw64_n64_render_queue_enqueue_static_mesh(fw64N64RenderQueue* render_queue, fw64MeshInstance* mesh_instance) {
    fw64_dynamic_vector_push_back(&render_queue->static_meshes, &mesh_instance);
}

void fw64_n64_render_queue_enqueue_skinned_mesh(fw64N64RenderQueue* render_queue, fw64SkinnedMeshInstance* mesh_instance) {
    fw64_dynamic_vector_push_back(&render_queue->skinned_meshes, &mesh_instance);
}
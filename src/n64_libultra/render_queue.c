#include "framework64/n64_libultra/render_queue.h"

#include <string.h>

void fw64_n64_render_queue_init(fw64N64RenderQueue* render_queue, fw64Allocator* allocator) {
    fw64_dynamic_vector_init(&render_queue->sprite_batches, sizeof(fw64SpriteBatch*), allocator);

    for (int i = 0; i < FW64_RENDER_QUEUE_INDEX_COUNT; i++) {
        fw64_dynamic_vector_init(&render_queue->meshes[i], sizeof(fw64MeshInstance*), allocator);
    }
}

void fw64_n64_render_queue_uninit(fw64N64RenderQueue* render_queue) {
    fw64_dynamic_vector_uninit(&render_queue->sprite_batches);

    for (int i = 0; i < FW64_RENDER_QUEUE_INDEX_COUNT; i++) {
        fw64_dynamic_vector_uninit(&render_queue->meshes[i]);
    }
}

void fw64_n64_render_queue_clear(fw64N64RenderQueue* render_queue) {
    fw64_dynamic_vector_clear(&render_queue->sprite_batches);

    for (int i = 0; i < FW64_RENDER_QUEUE_INDEX_COUNT; i++) {
        fw64_dynamic_vector_clear(&render_queue->meshes[i]);
    }
}

void fw64_n64_render_queue_enqueue_sprite_batch(fw64N64RenderQueue* render_queue, fw64SpriteBatch* sprite_batch) {
    fw64_dynamic_vector_push_back(&render_queue->sprite_batches, &sprite_batch);
}

void fw64_n64_render_queue_enqueue_static_mesh(fw64N64RenderQueue* render_queue, fw64MeshInstance* mesh_instance) {
    fw64_dynamic_vector_push_back(&render_queue->meshes[fw64_mesh_get_render_queue_index(mesh_instance->mesh)], &mesh_instance);
}

void fw64_n64_render_queue_enqueue_skinned_mesh(fw64N64RenderQueue* render_queue, fw64SkinnedMeshInstance* mesh_instance) {
    fw64_dynamic_vector_push_back(&render_queue->meshes[fw64_mesh_get_render_queue_index(mesh_instance->skinned_mesh->mesh)], &mesh_instance);
}
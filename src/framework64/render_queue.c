#include "framework64/render_queue.h"

#include <string.h>

int fw64_render_queue_has_items(fw64RenderQueue* render_queue, fw64ShadingMode index) {
    fw64RenderQueueBucket* bucket = &render_queue->buckets[index];
    return fw64_dynamic_vector_size(&bucket->static_) > 0 || fw64_dynamic_vector_size(&bucket->skinned_) > 0;
}

int fw64_render_queue_has_sprite_batches(fw64RenderQueue* render_queue) {
    return fw64_dynamic_vector_size(&render_queue->sprite_batches) > 0;
}

void fw64_render_queue_init(fw64RenderQueue* render_queue, fw64Allocator* allocator) {
    fw64_dynamic_vector_init(&render_queue->sprite_batches, sizeof(fw64SpriteBatch*), allocator);

    for (int i = 0; i < FW64_SHADING_MODE_COUNT; i++) {
        fw64_dynamic_vector_init(&render_queue->buckets[i].static_, sizeof(fw64StaticDrawInfo), allocator);
        fw64_dynamic_vector_init(&render_queue->buckets[i].skinned_, sizeof(fw64SkinnedDrawInfo), allocator);
    }
}

void fw64_render_queue_uninit(fw64RenderQueue* render_queue) {
    fw64_dynamic_vector_uninit(&render_queue->sprite_batches);

    for (int i = 0; i < FW64_SHADING_MODE_COUNT; i++) {
        fw64_dynamic_vector_uninit(&render_queue->buckets[i].static_);
        fw64_dynamic_vector_uninit(&render_queue->buckets[i].skinned_);
    }
}

void fw64_render_queue_clear(fw64RenderQueue* render_queue) {
    fw64_dynamic_vector_clear(&render_queue->sprite_batches);

    for (int i = 0; i < FW64_SHADING_MODE_COUNT; i++) {
        fw64_dynamic_vector_clear(&render_queue->buckets[i].static_);
        fw64_dynamic_vector_clear(&render_queue->buckets[i].skinned_);
    }
}

void fw64_render_queue_enqueue_sprite_batch(fw64RenderQueue* render_queue, fw64SpriteBatch* sprite_batch) {
    fw64_dynamic_vector_push_back(&render_queue->sprite_batches, &sprite_batch);
}

void fw64_render_queue_enqueue_static_mesh(fw64RenderQueue* render_queue, fw64MeshInstance* mesh_instance) {
    uint32_t primitive_count = fw64_mesh_get_primitive_count(mesh_instance->mesh);

    for (uint32_t i = 0; i < primitive_count; i++) {
        fw64Material* material = fw64_material_collection_get_material(mesh_instance->materials, i);
        fw64StaticDrawInfo* draw_info = fw64_dynamic_vector_emplace_back(&render_queue->buckets[fw64_material_get_shading_mode(material)].static_);
        draw_info->instance = mesh_instance;
        draw_info->index = i;
    }
}

void fw64_render_queue_enqueue_skinned_mesh(fw64RenderQueue* render_queue, fw64SkinnedMeshInstance* skinned_mesh_instance) {
        uint32_t primitive_count = fw64_mesh_get_primitive_count(skinned_mesh_instance->mesh_instance.mesh);

    for (uint32_t i = 0; i < primitive_count; i++) {
        fw64Material* material = fw64_material_collection_get_material(skinned_mesh_instance->mesh_instance.materials, i);
        fw64SkinnedDrawInfo* draw_info = fw64_dynamic_vector_emplace_back(&render_queue->buckets[fw64_material_get_shading_mode(material)].skinned_);
        draw_info->instance = skinned_mesh_instance;
        draw_info->index = i;
    }
}

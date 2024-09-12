#pragma once

#include "framework64/allocator.h"
#include "framework64/mesh_instance.h"
#include "framework64/skinned_mesh_instance.h"
#include "framework64/util/dynamic_vector.h"
#include "framework64/sprite_batch.h"

typedef struct {
    fw64MeshInstance* instance;
    uint32_t index;
} fw64StaticDrawInfo;

typedef struct {
    fw64SkinnedMeshInstance* instance;
    uint32_t index;
} fw64SkinnedDrawInfo;

typedef struct {
    fw64DynamicVector static_;
    fw64DynamicVector skinned_;
} fw64RenderQueueBucket;

typedef struct {
    fw64DynamicVector sprite_batches;
    fw64RenderQueueBucket buckets[FW64_SHADING_MODE_COUNT];
} fw64RenderQueue;

#ifdef __cplusplus
extern "C" {
#endif


void fw64_render_queue_init(fw64RenderQueue* render_queue, fw64Allocator* allocator);
void fw64_render_queue_uninit(fw64RenderQueue* render_queue);
void fw64_render_queue_clear(fw64RenderQueue* render_queue);

int fw64_render_queue_has_items(fw64RenderQueue* render_queue, fw64ShadingMode index);
int fw64_render_queue_has_sprite_batches(fw64RenderQueue* render_queue);

void fw64_render_queue_enqueue_sprite_batch(fw64RenderQueue* render_queue, fw64SpriteBatch* sprite_batch);
void fw64_render_queue_enqueue_static_mesh(fw64RenderQueue* render_queue, fw64MeshInstance* mesh_instance);
void fw64_render_queue_enqueue_skinned_mesh(fw64RenderQueue* render_queue, fw64SkinnedMeshInstance* skinned_mesh_instance);

#define fw64_render_queue_get_bucket(render_queue, queue_index) (&(render_queue)->buckets[queue_index])

#ifdef __cplusplus
}
#endif

#pragma once

#include "framework64/allocator.h"
#include "framework64/mesh_instance.h"
#include "framework64/skinned_mesh_instance.h"
#include "framework64/util/dynamic_vector.h"
#include "framework64/sprite_batch.h"

typedef struct {
    fw64DynamicVector sprite_batches;
    fw64DynamicVector static_meshes;
    fw64DynamicVector skinned_meshes;
} fw64N64RenderQueue;

void fw64_n64_render_queue_init(fw64N64RenderQueue* render_queue, fw64Allocator* allocator);
void fw64_n64_render_queue_uninit(fw64N64RenderQueue* render_queue);
void fw64_n64_render_queue_clear(fw64N64RenderQueue* render_queue);

void fw64_n64_render_queue_enqueue_sprite_batch(fw64N64RenderQueue* render_queue, fw64SpriteBatch* sprite_batch);
void fw64_n64_render_queue_enqueue_static_mesh(fw64N64RenderQueue* render_queue, fw64MeshInstance* mesh_instance);
void fw64_n64_render_queue_enqueue_skinned_mesh(fw64N64RenderQueue* render_queue, fw64SkinnedMeshInstance* mesh_instance);

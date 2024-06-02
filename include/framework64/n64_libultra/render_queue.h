#pragma once

#include "framework64/allocator.h"
#include "framework64/util/dynamic_vector.h"
#include "framework64/n64_libultra/sprite_batch.h"

// this is temporary
#include "framework64/mesh.h"
#include "framework64/transform.h"
#include "framework64/animation_data.h"
typedef struct {
    fw64Mesh* mesh;
    fw64Transform* transform;
} fw64N64MeshInstance;

// --

typedef struct {
    fw64DynamicVector sprite_batches;
    fw64DynamicVector static_meshes;
} fw64N64RenderQueue;

void fw64_n64_render_queue_init(fw64N64RenderQueue* render_queue, fw64Allocator* allocator);
void fw64_n64_render_queue_uninit(fw64N64RenderQueue* render_queue);
void fw64_n64_render_queue_clear(fw64N64RenderQueue* render_queue);

void fw64_n64_render_queue_enqueue_sprite_batch(fw64N64RenderQueue* render_queue, fw64SpriteBatch* sprite_batch);
void fw64_n64_render_queue_enqueue_static_mesh(fw64N64RenderQueue* render_queue,  fw64Mesh* mesh, fw64Transform* transform);

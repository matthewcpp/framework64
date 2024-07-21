#pragma once

#include "framework64/engine.h"
#include "framework64/render_pass.h"
#include "framework64/scene.h"
#include "framework64/static_vector.h"

typedef struct {
    fw64MeshInstance* src;
    fw64MeshInstance* dest;
} fw64DebugBoxEntry;

typedef struct {
    fw64Scene* scene;
    fw64Mesh* mesh;
    fw64StaticVector entries;
    fw64Allocator* allocator;
} fw64DebugBoxes;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_debug_boxes_init(fw64DebugBoxes* debug_boxes, int box_count, fw64Scene* scene, fw64Mesh* mesh, fw64Allocator* allocator);
void fw64_debug_boxes_uninit(fw64DebugBoxes* debug_boxes);

fw64Node* fw64_debug_boxes_add(fw64DebugBoxes* debug_boxes, fw64MeshInstance* source);

void fw64_debug_boxes_update(fw64DebugBoxes* debug_boxes);

#ifdef __cplusplus
}
#endif

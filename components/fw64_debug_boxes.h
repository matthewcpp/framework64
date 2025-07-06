#pragma once

#include "framework64/render_pass.h"
#include "framework64/scene.h"
#include "framework64/static_vector.h"

typedef struct {
    fw64Scene* scene;
    fw64Mesh* box_mesh;
    fw64StaticVector entries;
    fw64StaticVector wire_meshes;
    fw64Allocator* allocator;
} fw64DebugBoxes;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_debug_boxes_init(fw64DebugBoxes* debug_boxes, int debug_box_count, int wire_mesh_capacity, fw64Scene* scene, fw64Allocator* allocator);
void fw64_debug_boxes_uninit(fw64DebugBoxes* debug_boxes);

fw64Node* fw64_debug_boxes_add(fw64DebugBoxes* debug_boxes, fw64Node* node);

#ifdef __cplusplus
}
#endif

#pragma once

#include "framework64/camera.h"
#include "framework64/node.h"
#include "framework64/util/sparse_set.h"

typedef struct {
    fw64Node* node;
    fw64Camera* camera;
} fw64BillboardNode;

typedef fw64SparseSetHandle fw64BillboardNodeHandle;

typedef struct {
    fw64SparseSet components;
} fw64BillboardNodes;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_billboard_nodes_init(fw64BillboardNodes* system, fw64Allocator* allocator);
fw64BillboardNode* fw64_billboard_nodes_create(fw64BillboardNodes* system, fw64Node* node, fw64Camera* camera, fw64BillboardNodeHandle* out_handle);
void fw64_billboard_nodes_update(fw64BillboardNodes* system);

#ifdef __cplusplus
}
#endif

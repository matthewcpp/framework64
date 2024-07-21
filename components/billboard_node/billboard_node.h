#pragma once

#include "framework64/camera.h"
#include "framework64/node.h"
#include "framework64/util/sparse_set.h"

typedef struct {
    fw64Node* node;
    fw64Camera* camera;
} BillboardNode;

typedef fw64SparseSetHandle BillboardNodeHandle;

typedef struct {
    fw64SparseSet components;
} BillboardNodes;

#ifdef __cplusplus
extern "C" {
#endif

void billboard_nodes_init(BillboardNodes* system, fw64Allocator* allocator);
BillboardNode* billboard_nodes_create(BillboardNodes* system, fw64Node* node, fw64Camera* camera, BillboardNodeHandle* out_handle);
void billboard_nodes_update(BillboardNodes* system);

#ifdef __cplusplus
}
#endif

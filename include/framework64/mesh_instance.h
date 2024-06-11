#pragma once

#include "framework64/box.h"
#include "framework64/mesh.h"
#include "framework64/node.h"
#include "framework64/util/sparse_set.h"

typedef struct {
    fw64Node* node;
    fw64Mesh* mesh;
    Box render_bounds;
} fw64MeshInstance;

typedef struct {
    fw64SparseSet components;
} fw64MeshInstances;

typedef fw64SparseSetHandle fw64MeshInstanceHandle;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_mesh_instances_init(fw64MeshInstances* mesh_instances, fw64Allocator* allocator);
fw64MeshInstance* fw64_mesh_instances_create(fw64MeshInstances* mesh_instances, fw64Node* node, fw64Mesh* mesh, fw64MeshInstanceHandle* handle);

#ifdef __cplusplus
}
#endif

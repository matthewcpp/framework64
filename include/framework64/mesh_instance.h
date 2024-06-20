#pragma once

#include "framework64/animation_controller.h"
#include "framework64/box.h"
#include "framework64/component.h"
#include "framework64/mesh.h"
#include "framework64/node.h"
#include "framework64/util/sparse_set.h"

#ifdef FW64_PLATFORM_N64_LIBULTRA
    #include <nusys.h>
#endif

typedef struct {
    fw64Component component;
    fw64Node* node;
    fw64Mesh* mesh;
    Box render_bounds;

#ifdef FW64_PLATFORM_N64_LIBULTRA
    Mtx render_matrix;
#endif
} fw64MeshInstance;

typedef struct {
    fw64SparseSet components;
} fw64MeshInstances;



typedef fw64SparseSetHandle fw64MeshInstanceHandle;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_mesh_instance_init(fw64MeshInstance* mesh_instance, fw64Node* node, fw64Mesh* mesh);
void fw64_mesh_instance_update(fw64MeshInstance* mesh_instance);

void fw64_mesh_instances_init(fw64MeshInstances* mesh_instances, fw64Allocator* allocator);
fw64MeshInstance* fw64_mesh_instances_create(fw64MeshInstances* mesh_instances, fw64Node* node, fw64Mesh* mesh, fw64MeshInstanceHandle* handle);

#ifdef __cplusplus
}
#endif

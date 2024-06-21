#pragma once

/** \file skinned_mesh_instance.h */

#include "framework64/allocator.h"
#include "framework64/animation_controller.h"
#include "framework64/component.h"
#include "framework64/skinned_mesh.h"
#include "framework64/node.h"

#include "framework64/util/sparse_set.h"

typedef struct {
    fw64Component component;
    fw64Node* node;
    fw64SkinnedMesh* skinned_mesh;
    fw64AnimationController controller;
    Box render_bounds;
} fw64SkinnedMeshInstance;

typedef struct {
    fw64SparseSet components;
} fw64SkinnedMeshInstances;

typedef fw64SparseSetHandle fw64SkinnedMeshInstanceHandle;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_skinned_mesh_instance_init(fw64SkinnedMeshInstance* instance, fw64Node* node, fw64SkinnedMesh* skinned_mesh, int initial_animation, fw64Allocator* allocator);

void fw64_skinned_mesh_instances_init(fw64SkinnedMeshInstances* instances, fw64Allocator* allocator);
fw64SkinnedMeshInstance* fw64_skinned_mesh_instances_create(fw64SkinnedMeshInstances* instances, fw64Node* node, fw64SkinnedMesh* skinned_mesh, int intial_animation, fw64SkinnedMeshInstanceHandle* out_handle);
void fw64_skinned_mesh_instances_update(fw64SkinnedMeshInstances* instances, float time_delta);

#ifdef __cplusplus
}
#endif

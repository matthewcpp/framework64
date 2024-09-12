#pragma once

/** \file skinned_mesh_instance.h */

#include "framework64/allocator.h"
#include "framework64/animation_controller.h"
#include "framework64/mesh_instance.h"
#include "framework64/node.h"
#include "framework64/skinned_mesh.h"

#include "framework64/util/sparse_set.h"

typedef struct {
    fw64MeshInstance mesh_instance;
    fw64SkinnedMesh* skinned_mesh;
    fw64AnimationController controller;
} fw64SkinnedMeshInstance;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_skinned_mesh_instance_init(fw64SkinnedMeshInstance* instance, fw64Node* node, fw64SkinnedMesh* skinned_mesh, int initial_animation, fw64Allocator* allocator);
void fw64_skinned_mesh_uninit(fw64SkinnedMeshInstance* instance, fw64Allocator* allocator);
void fw64_skinned_mesh_instance_update(fw64SkinnedMeshInstance* instance, float time_delta);

#ifdef __cplusplus
}
#endif

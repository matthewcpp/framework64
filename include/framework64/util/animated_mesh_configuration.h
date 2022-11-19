#pragma once

#include "framework64/animation_controller.h"
#include "framework64/transform.h"
#include "framework64/mesh.h"

typedef struct {
    fw64Mesh** meshes;
    uint32_t* bone_indices;
    uint32_t capacity;
    uint32_t count;
} fw64AnimatedMeshConfiguration;

void fw64_animatied_mesh_configuration_init(fw64AnimatedMeshConfiguration* configuration, uint32_t capacity, fw64Allocator* allocator);
void fw64_animatied_mesh_configuration_uninit(fw64AnimatedMeshConfiguration* configuration, fw64Allocator* allocator);
void fw64_animatied_mesh_configuration_push(fw64AnimatedMeshConfiguration* configuration, fw64Mesh* mesh, uint32_t bone_index);
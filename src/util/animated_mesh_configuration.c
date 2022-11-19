#include "framework64/util/animated_mesh_configuration.h"

void fw64_animatied_mesh_configuration_init(fw64AnimatedMeshConfiguration* configuration, uint32_t capacity, fw64Allocator* allocator) {
    configuration->capacity = capacity;
    configuration->meshes = allocator->malloc(allocator, sizeof(fw64Mesh*) * capacity);
    configuration->bone_indices = allocator->malloc(allocator, sizeof(uint32_t) * capacity);
    configuration->count = 0;
}

void fw64_animatied_mesh_configuration_uninit(fw64AnimatedMeshConfiguration* configuration, fw64Allocator* allocator) {
    allocator->free(allocator, configuration->meshes);
    allocator->free(allocator, configuration->bone_indices);
}

void fw64_animatied_mesh_configuration_push(fw64AnimatedMeshConfiguration* configuration, fw64Mesh* mesh, uint32_t bone_index) {
    configuration->meshes[configuration->count] = mesh;
    configuration->bone_indices[configuration->count] = bone_index;

    configuration->count += 1;
}
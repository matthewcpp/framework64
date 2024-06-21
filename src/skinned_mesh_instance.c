#include "framework64/skinned_mesh_instance.h"

void fw64_skinned_mesh_instance_init(fw64SkinnedMeshInstance* instance, fw64Node* node, fw64SkinnedMesh* skinned_mesh, int initial_animation, fw64Allocator* allocator) {
    instance->node = node;
    instance->skinned_mesh = skinned_mesh;
    
    fw64_animation_controller_init(&instance->controller, skinned_mesh->animation_data, initial_animation, allocator);
}

void fw64_skinned_mesh_instances_init(fw64SkinnedMeshInstances* instances, fw64Allocator* allocator) {
    fw64_sparse_set_init(&instances->components, sizeof(fw64SkinnedMeshInstance), allocator);
}

fw64SkinnedMeshInstance* fw64_skinned_mesh_instances_create(fw64SkinnedMeshInstances* instances, fw64Node* node, fw64SkinnedMesh* skinned_mesh, int initial_animation, fw64SkinnedMeshInstanceHandle* out_handle) {
    fw64SkinnedMeshInstanceHandle handle;
    fw64SkinnedMeshInstance* skinned_mesh_instance = (fw64SkinnedMeshInstance*)fw64_sparse_set_alloc(&instances->components, &handle);

    fw64_skinned_mesh_instance_init(skinned_mesh_instance, node, skinned_mesh, initial_animation, instances->components.allocator);

    if (out_handle) {
        *out_handle = handle;
    }

    return skinned_mesh_instance;
}

void fw64_skinned_mesh_instances_update(fw64SkinnedMeshInstances* instances, float time_delta) {
    fw64SparseSetItr itr;
    for (fw64_sparse_set_itr_init(&itr, &instances->components); fw64_sparse_set_itr_has_next(&itr); fw64_sparse_set_itr_next(&itr)) {
        fw64SkinnedMeshInstance* skinned_mesh_instance  = (fw64SkinnedMeshInstance*)(fw64_sparse_set_itr_get_item(&itr));

        fw64_animation_controller_update(&skinned_mesh_instance->controller, time_delta);
    }
}

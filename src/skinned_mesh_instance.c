#include "framework64/skinned_mesh_instance.h"

void fw64_skinned_mesh_instance_init(fw64SkinnedMeshInstance* instance, fw64Node* node, fw64SkinnedMesh* skinned_mesh, int initial_animation, fw64Allocator* allocator) {
    fw64_mesh_instance_init((fw64MeshInstance*)instance, node, skinned_mesh->mesh);
    instance->mesh_instance.base.type_id = FW64_COMPONENT_TYPE_SKINNED_MESH_INSTANCE;
    instance->skinned_mesh = skinned_mesh;
    
    fw64_animation_controller_init(&instance->controller, skinned_mesh->animation_data, initial_animation, allocator);
}

void fw64_skinned_mesh_instance_update(fw64SkinnedMeshInstance* instance, float time_delta) {
    fw64_animation_controller_update(&instance->controller, time_delta);
    //fw64_mesh_instance_update((fw64MeshInstance*)instance);
}

void fw64_skinned_mesh_instance_components_init(fw64SkinnedMeshInstanceComponents* components, fw64Allocator* allocator) {
    fw64_sparse_set_init(&components->set, sizeof(fw64SkinnedMeshInstance), allocator);
}

fw64SkinnedMeshInstance* fw64_skinned_mesh_instance_components_create(fw64SkinnedMeshInstanceComponents* components, fw64Node* node, fw64SkinnedMesh* skinned_mesh, fw64SparseSetHandle* out_handle) {
    fw64SparseSetHandle handle;
    fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_sparse_set_alloc(&components->set, &out_handle);

    fw64_skinned_mesh_instance_init(skinned_mesh_instance, node, skinned_mesh, 0, fw64_sparse_set_get_allocator(&components->set));

    if (out_handle) {
        *out_handle = handle;
    }

    return skinned_mesh_instance;
}

void fw64_skinned_mesh_instance_components_update(fw64SkinnedMeshInstanceComponents* components, float time_delta) {

}
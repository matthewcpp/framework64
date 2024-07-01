#include "framework64/skinned_mesh_instance.h"

void fw64_skinned_mesh_instance_init(fw64SkinnedMeshInstance* instance, fw64Node* node, fw64SkinnedMesh* skinned_mesh, int initial_animation, fw64Allocator* allocator) {
    fw64_mesh_instance_init((fw64MeshInstance*)instance, node, skinned_mesh->mesh);
    instance->mesh_instance.base.type_id = FW64_COMPONENT_TYPE_SKINNED_MESH_INSTANCE;
    instance->skinned_mesh = skinned_mesh;
    
    fw64_animation_controller_init(&instance->controller, skinned_mesh->animation_data, initial_animation, allocator);
}

void fw64_skinned_mesh_instance_update(fw64SkinnedMeshInstance* instance, float time_delta) {
    if (instance->mesh_instance.base.node == NULL) {
        return;
    }

    fw64_animation_controller_update(&instance->controller, time_delta);
    //fw64_mesh_instance_update((fw64MeshInstance*)instance);
}

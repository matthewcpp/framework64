#include "framework64/registry.h"

void fw64_registry_init(fw64Registry* registry, fw64Allocator* allocator) {
    fw64_dynamic_vector_init(&registry->components_vec, sizeof(fw64Components*), allocator);


}

int fw64_registry_add(fw64Registry* registry, fw64Components* components) {
    fw64_dynamic_vector_push_back(&registry->components_vec, &components);
    return 1;
}

fw64Components* fw64_registry_get_components(fw64Registry* registry, uint32_t component_id) {
    for (size_t i = 0; i < fw64_dynamic_vector_size(&registry->components_vec); i++) {
        fw64Components* components = (fw64Components*)fw64_dynamic_vector_item(&registry->components_vec, i);
        
        if (components->type_id == component_id) {
            return components;
        }
    }

    return NULL;
}

fw64SkinnedMeshInstances* fw64_registry_get_skinned_mesh_instances(fw64Registry* registry) {
    fw64SkinnedMeshInstances* instances = (fw64SkinnedMeshInstances*)fw64_registry_get_components(registry, FW64_COMPONENT_TYPE_SKINNED_MESH_INSTANCE);

    if (!instances) {
        fw64Allocator* allocator = registry->components_vec._allocator;
        instances = allocator->malloc(allocator, sizeof(fw64SkinnedMeshInstances));
        fw64_skinned_mesh_instances_init(instances, allocator);

        fw64_registry_add(registry, &instances->base);
    }

    return instances;
}

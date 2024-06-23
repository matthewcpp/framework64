#pragma once

#include "framework64/component.h"
#include "framework64/node.h"
#include "framework64/util/dynamic_vector.h"

#include "framework64/skinned_mesh_instance.h"

typedef void (*fw64ComponentAdded)(fw64Node* node, fw64Component* component, void* arg);

typedef struct {
    fw64DynamicVector components_vec;
} fw64Registry;

void fw64_registry_init(fw64Registry* registry, fw64Allocator* allocator);
int fw64_registry_add(fw64Registry* registry, fw64Components* components);
fw64Components* fw64_registry_get_components(fw64Registry* registry, uint32_t component_id);

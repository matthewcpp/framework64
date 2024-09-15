#include "framework64/node.h"

#include "framework64/mesh_instance.h"

void fw64_node_init(fw64Node* node) {
    fw64_transform_init(&node->transform);
    node->mesh_instance = NULL;
    node->collider = NULL;
    node->layer_mask = FW64_DEFAULT_LAYER_MASK;
    node->data = 0;
}

void fw64_node_update(fw64Node* node) {
    fw64_transform_update_matrix(&node->transform);

    if (node->collider) {
        fw64_collider_update(node->collider);
    }

    if (node->mesh_instance) {
        fw64_mesh_instance_update(node->mesh_instance);
    }
}

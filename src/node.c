#include "framework64/node.h"

#include "framework64/matrix.h"


void fw64_node_init(fw64Node* node, fw64Mesh* mesh) {
    fw64_transform_init(&node->transform);
    fw64_collider_init(&node->collider, &node->transform);
    fw64_node_set_mesh(node, mesh);
    node->type = FW64_NODE_UNSPECIFIED_TYPE;
    node->layer_mask = 1U;
}

void fw64_node_update(fw64Node* node) {
    fw64_transform_update_matrix(&node->transform);
    fw64_collider_update(&node->collider);
}

void fw64_node_set_mesh(fw64Node* node, fw64Mesh* mesh) {
    node->mesh = mesh;

    if (node->mesh == NULL) {
        fw64_collider_set_type_none(&node->collider);
        return;
    }

    Box box;
    fw64_mesh_get_bounding_box(node->mesh, &box);
    fw64_collider_set_type_box(&node->collider, &box);
}

void fw64_node_billboard(fw64Node* node, fw64Camera* camera) {
    Vec3 camera_forward, camera_up, target;
    fw64_transform_forward(&camera->transform, &camera_forward);
    fw64_transform_up(&camera->transform, &camera_up);

    vec3_add(&target, &node->transform.position, &camera_forward);
    fw64_transform_look_at(&node->transform, &target, &camera_up);

    fw64_node_update(node);
}
#include "framework64/node.h"


void fw64_node_init(fw64Node* node) {
    fw64_transform_init(&node->transform);
    node->mesh = NULL;
    node->collider = NULL;
    node->type = FW64_NODE_UNSPECIFIED_TYPE;
    node->layer_mask = 1U;
}

void fw64_node_update(fw64Node* node) {
    fw64_transform_update_matrix(&node->transform);
    if (node->collider)
        fw64_collider_update(node->collider);
}

static void setup_node_collider(fw64Node* node) {
    if (node->mesh == NULL) {
        fw64_collider_set_type_none(node->collider);
    }
    else {
        Box box;
        fw64_mesh_get_bounding_box(node->mesh, &box);
        fw64_collider_set_type_box(node->collider, &box);
    }
}

void fw64_node_set_collider(fw64Node* node, fw64Collider* collider) {
    node->collider = collider;
    fw64_collider_init(node->collider, &node->transform);
    setup_node_collider(node);
}

void fw64_node_set_mesh(fw64Node* node, fw64Mesh* mesh) {
    node->mesh = mesh;

    if (!node->collider)
        return;

    setup_node_collider(node);
}

void fw64_node_billboard(fw64Node* node, fw64Camera* camera) {
    Vec3 camera_forward, camera_up, target;
    fw64_transform_forward(&camera->transform, &camera_forward);
    fw64_transform_up(&camera->transform, &camera_up);

    vec3_add(&target, &node->transform.position, &camera_forward);
    fw64_transform_look_at(&node->transform, &target, &camera_up);

    fw64_node_update(node);
}
#include "framework64/node.h"

#include "framework64/mesh_instance.h"

void fw64_node_init(fw64Node* node) {
    fw64_transform_init(&node->transform);
    node->components = NULL;
    node->collider = NULL;
    node->layer_mask = 1U;
    node->data = NULL;
}

void fw64_node_update(fw64Node* node) {
    fw64_transform_update_matrix(&node->transform);

    if (node->collider)
        fw64_collider_update(node->collider);
}

void fw64_node_add_componet(fw64Node* node, fw64Component* component) {
    component->next = node->components;
    node->components = component;
}

fw64Component* fw64_node_find_component(fw64Node* node, fw64ComponentTypeId component_type_id) {
    fw64Component* current = node->components;

    while (current) {
        if (current->type_id == component_type_id) {
            return current;
        }
    }

    return NULL;
}

// TODO: Temp implementation
static void setup_box_collider(fw64Node* node) {
    fw64MeshInstance* mesh_instance = (fw64MeshInstance*)fw64_node_find_component(node, FW64_COMPONENT_TYPE_MESH_INSTANCE);

    Box box = fw64_mesh_get_bounding_box(mesh_instance->mesh);
    fw64_collider_set_type_box(node->collider, &box);
}

void fw64_node_set_collider(fw64Node* node, fw64Collider* collider) {
    node->collider = collider;
    fw64_collider_init(node->collider, &node->transform);
}

void fw64_node_set_box_collider(fw64Node* node, fw64Collider* collider) {
    fw64_node_set_collider(node, collider);
    fw64MeshInstance* mesh_instance = (fw64MeshInstance*)fw64_node_find_component(node, FW64_COMPONENT_TYPE_MESH_INSTANCE);
    
    if (mesh_instance != NULL)
        setup_box_collider(node);
}

void fw64_node_set_mesh_collider(fw64Node* node, fw64Collider* collider, fw64CollisionMesh* collision_mesh) {
    fw64_node_set_collider(node, collider);
    fw64_collider_set_type_mesh(node->collider, collision_mesh);
}

void fw64_node_billboard(fw64Node* node, fw64Camera* camera) {
    Vec3 camera_forward, camera_up, target;
    fw64_transform_forward(&camera->transform, &camera_forward);
    fw64_transform_up(&camera->transform, &camera_up);

    vec3_add(&target, &node->transform.position, &camera_forward);
    fw64_transform_look_at(&node->transform, &target, &camera_up);

    fw64_node_update(node);
}

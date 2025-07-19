#include "fw64_debug_boxes.h"

#include "fw64_wireframe.h"

void fw64_debug_boxes_init(fw64DebugBoxes* debug_boxes, int debug_box_count, int wire_mesh_count, fw64Scene* scene, fw64Allocator* allocator) {
    debug_boxes->scene = scene;
    debug_boxes->allocator = allocator;
    
    Vec3 center, extents;
    vec3_set_zero(&center);
    vec3_set_one(&extents);
    Box box;
    box_set_center_extents(&box, &center, &extents);

    debug_boxes->box_mesh = fw64_wireframe_build_box(&box, scene->assets, allocator);

    fw64_static_vector_init(&debug_boxes->entries, sizeof(fw64Node*), debug_box_count, allocator);
    fw64_static_vector_init(&debug_boxes->wire_meshes, sizeof(fw64Mesh*), wire_mesh_count, allocator);
}

void fw64_debug_boxes_uninit(fw64DebugBoxes* debug_boxes) {
    for (uint32_t i = 0; i < fw64_static_vector_size(&debug_boxes->wire_meshes); i++) {
        fw64_mesh_delete((fw64Mesh*)fw64_static_vector_get_item(&debug_boxes->wire_meshes, i), debug_boxes->scene->assets, debug_boxes->allocator);
    }
    fw64_static_vector_uninit(&debug_boxes->wire_meshes, debug_boxes->allocator);

    fw64_static_vector_uninit(&debug_boxes->entries, debug_boxes->allocator);
    fw64_mesh_delete(debug_boxes->box_mesh, debug_boxes->scene->assets, debug_boxes->allocator);
}

fw64Node* fw64_debug_boxes_add(fw64DebugBoxes* debug_boxes, fw64Node* node) {
    if (!node->transform.parent) {
        return NULL;
    }

    fw64Node* bounding_node = fw64_scene_create_node(debug_boxes->scene);

    if (!bounding_node) {
        return NULL;
    }

    box_center(&node->collider->source, &bounding_node->transform.position);
    box_extents(&node->collider->source, &bounding_node->transform.scale);

    fw64Mesh* mesh = debug_boxes->box_mesh;
    if (node->collider->type == FW64_COLLIDER_COLLISION_MESH) {
        mesh = fw64_wireframe_build_collision_mesh(node->collider->collision_mesh, debug_boxes->scene->assets, debug_boxes->allocator);
        fw64_static_vector_push_back(&debug_boxes->wire_meshes, &mesh);
    }

    if (!fw64_scene_create_mesh_instance(debug_boxes->scene, bounding_node, mesh)) {
        return NULL;
    }

    fw64_transform_add_child(&node->transform, &bounding_node->transform);
    fw64_node_update(bounding_node);

    fw64_static_vector_push_back(&debug_boxes->entries, &bounding_node);

    return bounding_node;
}

void fw64_debug_boxes_update(fw64DebugBoxes* debug_boxes) {
    for (size_t i = 0; i < fw64_static_vector_size(&debug_boxes->entries); i++) {
        fw64Node* node = *((fw64Node**)fw64_static_vector_get_item(&debug_boxes->entries, i));
        fw64_mesh_instance_update(node->mesh_instance);
    }
}


#include "framework64/node.h"

#include "framework64/matrix.h"

static void _node_update_bounding_box_with_matrix(fw64Node* node, float* matrix) {
    if (node->mesh) {
        Box mesh_bounding_box;
        fw64_mesh_get_bounding_box(node->mesh, &mesh_bounding_box);
        matrix_transform_box(matrix, &mesh_bounding_box, &node->bounding);
    }
}

void fw64_node_init(fw64Node* node, fw64Mesh* mesh) {
    fw64_transform_init(&node->transform);
    fw64_node_set_mesh(node, mesh);
    node->type = FW64_NODE_UNSPECIFIED_TYPE;
    node->layer_mask = 1U;
}

void fw64_node_refresh(fw64Node* node) {
#ifdef PLATFORM_N64
    float fmatrix[16];
    matrix_from_trs(fmatrix, &node->transform.position, &node->transform.rotation, &node->transform.scale);

    guMtxF2L((float (*)[4])fmatrix, &node->transform.matrix);

    _node_update_bounding_box_with_matrix(node, &fmatrix[0]);
#else
    fw64_transform_update_matrix(&node->transform);
    _node_update_bounding_box_with_matrix(node, &node->transform.matrix.m[0]);
#endif
}

void fw64_node_set_mesh(fw64Node* node, fw64Mesh* mesh) {
    node->mesh = mesh;

    if (mesh) {
        fw64_node_refresh(node);
    }
    else {
        node->bounding.min = node->transform.position;
        node->bounding.max = node->transform.position;
    }
}

void fw64_node_billboard(fw64Node* node, fw64Camera* camera) {
    Vec3 camera_forward, camera_up, target;
    fw64_transform_forward(&camera->transform, &camera_forward);
    fw64_transform_up(&camera->transform, &camera_up);

    vec3_add(&target, &node->transform.position, &camera_forward);
    fw64_transform_look_at(&node->transform, &target, &camera_up);

    fw64_node_refresh(node);
}

void fw64_node_sphere(fw64Node* node, Vec3* center, float* radius) {
    box_center(&node->bounding, center);
    *radius = vec3_distance(center, &node->bounding.max);

    Vec3 extents;
    box_extents(&node->bounding, &extents);
    *radius = extents.y * 2;
}
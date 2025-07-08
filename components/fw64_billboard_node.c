#include "fw64_billboard_node.h"

void fw64_billboard_nodes_init(fw64BillboardNodes* system, fw64Allocator* allocator) {
    fw64_sparse_set_init(&system->components, sizeof(fw64BillboardNode), allocator);
}

fw64BillboardNode* fw64_billboard_nodes_create(fw64BillboardNodes* system, fw64Node* node, fw64Camera* camera, fw64BillboardNodeHandle* out_handle) {
    fw64BillboardNodeHandle handle;
    fw64BillboardNode* billboard_node = (fw64BillboardNode*)fw64_sparse_set_alloc(&system->components, &handle);

    billboard_node->node = node;
    billboard_node->camera = camera;

    if (out_handle) {
        *out_handle = handle;
    }

    return billboard_node;
}

void fw64_billboard_nodes_update(fw64BillboardNodes* system) {
    fw64SparseSetItr itr;
    for (fw64_sparse_set_itr_init(&itr, &system->components); fw64_sparse_set_itr_has_next(&itr); fw64_sparse_set_itr_next(&itr)) {
        fw64BillboardNode* billboard = (fw64BillboardNode*)fw64_sparse_set_itr_get_item(&itr);

        Vec3 camera_back, camera_up, target;
        fw64_transform_back(&billboard->camera->node->transform, &camera_back);
        fw64_transform_up(&billboard->camera->node->transform, &camera_up);

        vec3_add(&billboard->node->transform.position, &camera_back, &target);
        fw64_transform_look_at(&billboard->node->transform, &target, &camera_up);

        fw64_node_update(billboard->node);
    }
}

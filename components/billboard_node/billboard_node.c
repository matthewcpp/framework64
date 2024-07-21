#include "billboard_node.h"

void billboard_nodes_init(BillboardNodes* system, fw64Allocator* allocator) {
    fw64_sparse_set_init(&system->components, sizeof(BillboardNode), allocator);
}

BillboardNode* billboard_nodes_create(BillboardNodes* system, fw64Node* node, fw64Camera* camera, BillboardNodeHandle* out_handle) {
    BillboardNodeHandle handle;
    BillboardNode* billboard_node = (BillboardNode*)fw64_sparse_set_alloc(&system->components, &handle);

    billboard_node->node = node;
    billboard_node->camera = camera;

    if (out_handle) {
        *out_handle = handle;
    }

    return billboard_node;
}

void billboard_nodes_update(BillboardNodes* system) {
    fw64SparseSetItr itr;
    for (fw64_sparse_set_itr_init(&itr, &system->components); fw64_sparse_set_itr_has_next(&itr); fw64_sparse_set_itr_next(&itr)) {
        BillboardNode* billboard = (BillboardNode*)fw64_sparse_set_itr_get_item(&itr);

        Vec3 camera_forward, camera_up, target;
        fw64_transform_forward(&billboard->camera->transform, &camera_forward);
        fw64_transform_up(&billboard->camera->transform, &camera_up);

        vec3_add(&target, &billboard->node->transform.position, &camera_forward);
        fw64_transform_look_at(&billboard->node->transform, &target, &camera_up);

        fw64_node_update(billboard->node);
    }
}

#include "fw64_scene_partition_default.h"

static int fw64_scene_partition_default_overlap_box(fw64SceneParition* p, const Box* box, uint32_t layer_mask, fw64OverlapBoxQueryResult* result);
static int fw64_scene_partition_default_overlap_sphere(fw64SceneParition* p, const Vec3* center, float radius, uint32_t layer_mask, fw64OverlapSphereQueryResult* result);

void fw64_scene_partition_default_init(fw64ScenePartitionDefault* partition, fw64Scene* scene) {
    partition->base.overlap_box = fw64_scene_partition_default_overlap_box;
    partition->base.overlap_sphere = fw64_scene_partition_default_overlap_sphere;
    partition->scene = scene;
}

int fw64_scene_partition_default_overlap_box(fw64SceneParition* p, const Box* box, uint32_t layer_mask, fw64OverlapBoxQueryResult* result) {
    fw64ScenePartitionDefault* partition = (fw64ScenePartitionDefault*)p;
    result->count = 0;

    uint32_t node_count = fw64_scene_get_node_count(partition->scene);

    for (uint32_t i = 0; i < node_count; i++) {
        const fw64Node* node = fw64_scene_get_node(partition->scene, i);

        if (!node->collider || !(node->layer_mask & layer_mask) || !fw64_collider_is_active(node->collider))
            continue;

        if (fw64_collider_test_box(node->collider, box)) {
            result->colliders[result->count++] = node->collider;
        }

        // todo: probably a better way to handle this?
        if (result->count == Fw64_COLLISION_QUERY_RESULT_SIZE)
            break;
    }

    return result->count;
}

int fw64_scene_partition_default_overlap_sphere(fw64SceneParition* p, const Vec3* center, float radius, uint32_t layer_mask, fw64OverlapSphereQueryResult* result) {
    fw64ScenePartitionDefault* partition = (fw64ScenePartitionDefault*)p;

    result->count = 0;
    fw64OverlapSphereResult* sphere_hit = &result->results[0];

    uint32_t node_count = fw64_scene_get_node_count(partition->scene);

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(partition->scene, i);

        if (!node->collider || !(node->layer_mask & layer_mask) || !fw64_collider_is_active(node->collider))
            continue;

        if (fw64_collider_test_sphere(node->collider, center, radius, &sphere_hit->point)) {
            sphere_hit->node = node;
            sphere_hit = &result->results[++result->count];
        }

        // todo: probably a better way to handle this?
        if (result->count == Fw64_COLLISION_QUERY_RESULT_SIZE)
            break;
    }

    return result->count;
}

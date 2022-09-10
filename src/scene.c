#include "framework64/scene.h"

void fw64_scene_draw_all(fw64Scene* scene, fw64Renderer* renderer) {
    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (uint32_t i = 0 ; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->mesh)
            continue;

        fw64_renderer_draw_static_mesh(renderer, &node->transform, node->mesh);
    }
}

void fw64_scene_draw_frustrum(fw64Scene* scene, fw64Renderer* renderer, fw64Frustum* frustum) {
    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (uint32_t i = 0 ; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->mesh || !node->collider) // TODO: requiring collider may not be the most ideal setup
            continue;

        if (fw64_frustum_intersects_box(frustum, &node->collider->bounding)) {
            fw64_renderer_draw_static_mesh(renderer, &node->transform, node->mesh);
        }
    }
}

int fw64_scene_raycast(fw64Scene* scene, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit) {
    uint32_t node_count = fw64_scene_get_node_count(scene);
    hit->distance = FLT_MAX;
    hit->node = NULL;

    Vec3 hit_pos;
    float dist;

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->collider || !(mask & node->layer_mask))
            continue;

        if (fw64_collider_test_ray(node->collider, origin, direction, &hit_pos, &dist)) {
            if (dist > hit->distance) continue;

            hit->node = node;
            hit->point = hit_pos;
            hit->distance = dist;
        }
    }

    return hit->node != NULL;
}

int fw64_scene_overlap_sphere(fw64Scene* scene, Vec3* center, float radius, uint32_t mask, fw64OverlapSphereQueryResult* result) {
    result->count = 0;
    fw64OverlapSphereResult* sphere_hit = &result->results[0];

    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (int i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->collider || !(node->layer_mask & mask))
            continue;

        if (fw64_collider_test_sphere(node->collider, center, radius, &sphere_hit->point)) {
            sphere_hit->node = node;
            sphere_hit = &result->results[++result->count];
        }
    }

    return result->count;
}

int fw64_scene_moving_box_intersection(fw64Scene* scene, Box* box, Vec3* velocity, uint32_t mask, fw64IntersectMovingBoxQuery* result) {
    Vec3 static_vel = {0.0f, 0.0f, 0.0f};
    result->count = 0;
    IntersectMovingBoxResult* box_intersect = &result->results[0];

    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (int i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->collider || !(node->layer_mask & mask))
            continue;

        if (fw64_collision_test_moving_boxes(&node->collider->bounding, &static_vel, box, velocity, &box_intersect->tfirst, &box_intersect->tlast)) {
            box_intersect->node = node;
            box_intersect = &result->results[++result->count];
        }

        // temp
        if (result->count == 5)
            break;
    }

    return result->count;
}

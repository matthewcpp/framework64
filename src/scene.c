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

int fw64_scene_raycast(fw64Scene* scene, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit) {
    uint32_t node_count = fw64_scene_get_node_count(scene);
    hit->distance = FLT_MAX;

    Vec3 hit_pos;
    float dist;

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if ((mask & node->layer_mask) == 0)
            continue;

        if (fw64_collision_test_ray_box(origin, direction, &node->bounding, &hit_pos, &dist)) {
            if (dist > hit->distance) continue;

            hit->point = hit_pos;
            hit->distance = dist;
        }
    }

    return hit->distance != FLT_MAX;
}

int fw64_scene_overlap_sphere(fw64Scene* scene, Vec3* center, float radius, uint32_t mask, fw64OverlapSphereQueryResult* result) {
    result->count = 0;
    fw64OverlapSphereResult* sphere_hit = &result->results[0];

    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (int i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (fw64_collision_test_box_sphere(&node->bounding, center, radius, &sphere_hit->point)) {
            sphere_hit->node = node;
            sphere_hit = &result->results[++result->count];
        }
    }

    return result->count;
}
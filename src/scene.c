#include "framework64/scene.h"

void fw64_scene_draw_all(fw64Scene* scene, fw64Renderer* renderer) {
    uint32_t node_count = fw64_scene_get_mesh_count(scene);

    for (uint32_t i = 0 ; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->mesh)
            continue;

        fw64_renderer_draw_static_mesh(renderer, &node->transform, node->mesh);
    }
}

int fw64_scene_raycast(fw64Scene* scene, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit) {
    uint32_t mesh_count = fw64_scene_get_mesh_count(scene);

    hit->distance = FLT_MAX;

    for (int i = 0; i < mesh_count; i++) {
        fw64Mesh* mesh = fw64_scene_get_mesh(scene, i);
        int primitive_count = fw64_mesh_get_primitive_count(mesh);

        for (int j = 0; j < primitive_count; j++) {
            Vec3 hit_point;
            float t;
            Box bounding = fw64_mesh_get_bounding_for_primitive(mesh, j);
            if (fw64_collision_test_ray_box(origin, direction, &bounding, &hit_point, &t)) {

                if (t < hit->distance) {
                    hit->distance = t;
                    hit->point = hit_point;
                }
            }
        }
    }

    return hit->distance != FLT_MAX;
}

int fw64_scene_overlap_sphere(fw64Scene* scene, Vec3* center, float radius, uint32_t mask, fw64OverlapSphereQueryResult* result) {
    result->count = 0;
    fw64OverlapSphereResult* sphere_hit = &result->results[0];

    uint32_t mesh_count = fw64_scene_get_mesh_count(scene);

    for (int i = 0; i < mesh_count; i++) {
        fw64Mesh* mesh = fw64_scene_get_mesh(scene, i);
        int primitive_count = fw64_mesh_get_primitive_count(mesh);

        for (int j = 0; j < primitive_count; j++) {
            Box bounding = fw64_mesh_get_bounding_for_primitive(mesh, j);

            if (fw64_collision_test_box_sphere(&bounding, center, radius, &sphere_hit->point)) {
                sphere_hit->mesh = mesh;
                sphere_hit->primitive = j;

                sphere_hit = &result->results[++result->count];
            }
        }
    }

    return result->count;
}
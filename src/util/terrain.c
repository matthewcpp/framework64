#include "framework64/util/terrain.h"

int fw64_terrain_draw(fw64Terrain* terrain, fw64Renderer* renderer) {
    fw64Transform* transform = fw64_terrain_get_transform(terrain);
    uint32_t mesh_count = fw64_terrain_get_mesh_count(terrain);
    int draw_count = 0;

    fw64Camera* camera = fw64_renderer_get_camera(renderer);
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(camera, &frustum);

    Box bounding;
    Vec3 center;
    for (uint32_t i = 0; i < mesh_count; i++) {
        fw64Mesh* mesh = fw64_terrain_get_mesh(terrain, i);
        fw64_mesh_get_bounding_box(mesh, &bounding);
        box_center(&bounding, &center);
        
        if (fw64_frustum_intersects_box(&frustum, &bounding)) {
            fw64_renderer_draw_static_mesh(renderer, transform, mesh);
            draw_count += 1;
        }
    }

    return draw_count;
}

float fw64_terrain_get_height(fw64Terrain* terrain, float x, float z) {
    return 0.0f;
}
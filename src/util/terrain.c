#include "framework64/util/terrain.h"

void fw64_terrain_draw(fw64Terrain* terrain, fw64Renderer* renderer) {
    fw64Camera* camera = fw64_renderer_get_camera(renderer);
    fw64Transform* transform = fw64_terrain_get_transform(terrain);

    uint32_t mesh_count = fw64_terrain_get_mesh_count(terrain);

    for (uint32_t i = 0; i < mesh_count; i++) {
        fw64Mesh* mesh = fw64_terrain_get_mesh(terrain, i);
        fw64_renderer_draw_static_mesh(renderer, transform, mesh);
    }
}

float fw64_terrain_get_height(fw64Terrain* terrain, float x, float z) {
    return 0.0f;
}
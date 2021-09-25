#include "framework64/util/terrain.h"

void fw64_terrain_draw(fw64Terrain* terrain, fw64Renderer* renderer) {
    fw64Camera* camera = fw64_renderer_get_camera(renderer);

    for (size_t i = 0; i < terrain->mesh_count; i++) {
        fw64Mesh* mesh = terrain->meshes[i];
        fw64_renderer_draw_static_mesh(renderer, &terrain->transform, mesh);
    }
}

float fw64_terrain_get_height(fw64Terrain* terrain, float x, float z) {
    return 0.0f;
}
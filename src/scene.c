#include "framework64/scene.h"

void fw64_scene_draw_all(fw64Scene* scene, fw64Renderer* renderer) {
    uint32_t mesh_count = fw64_scene_get_mesh_count(scene);
    fw64Transform* transform = fw64_scene_get_transform(scene);

    for (uint32_t i = 0 ; i < mesh_count; i++) {
        fw64_renderer_draw_static_mesh(renderer, transform, fw64_scene_get_mesh(scene, i));
    }
}
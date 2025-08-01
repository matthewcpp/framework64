#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;

    fw64SceneInfo scene_info;
    fw64_scene_info_init(&scene_info);
    scene_info.node_count = 2;
    scene_info.mesh_count = 1;
    scene_info.mesh_instance_count = 1;

    fw64_scene_init(&game->scene, &scene_info, engine->assets, allocator);
    fw64Node* node = fw64_scene_create_node(&game->scene);
    vec3_set_all(&node->transform.scale, 0.1f);
    fw64_node_update(node);

    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_n64_logo);
    fw64_scene_create_mesh_instance(&game->scene, node, mesh);

    fw64_rotate_node_init(&game->rotate_node, node);

    fw64Node* camera_node = fw64_scene_create_node(&game->scene);
    vec3_set(&camera_node->transform.position, 0.0f, 7.5f, 18.0f);
    Vec3 target = {0.0f, 0.0f, 0.0f};
    Vec3 up = vec3_up();
    fw64_transform_look_at(&camera_node->transform, &target, &up);
    fw64_node_update(camera_node);

    fw64Camera camera;
    fw64_camera_init(&camera, camera_node, display);

    game->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_camera(game->renderpass, &camera);
}

void game_update(Game* game){
    fw64_rotate_node_update(&game->rotate_node, game->engine->time->time_delta);
}

void game_fixed_update(Game* game) {
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderpass_begin(game->renderpass);
    fw64_scene_draw_all(&game->scene, game->renderpass);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
}

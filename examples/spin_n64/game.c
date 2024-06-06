#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

#define ROTATION_SPEED 90.0f

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;

    fw64SceneInfo scene_info;
    fw64_scene_info_init(&scene_info);
    scene_info.mesh_count = 1;
    scene_info.node_count = 1;

    fw64_scene_init(&game->scene, &scene_info, engine->assets, allocator);
    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_n64_logo, 0);
    game->node = fw64_scene_get_node(&game->scene, 0);
    vec3_set_all(&game->node->transform.scale, 0.1f);
    fw64_node_set_mesh(game->node, mesh);

    game->rotation = 0.0f;

    fw64Camera camera;
    fw64_camera_init(&camera, display);
    vec3_set(&camera.transform.position, 0.0f, 7.5f, 18.0f);
    Vec3 target = {0.0f, 0.0f, 0.0f};
    Vec3 up = {0.0f, 1.0f, 0.0f};
    fw64_transform_look_at(&camera.transform, &target, &up);
    fw64_camera_update_view_matrix(&camera);

    game->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_camera(game->renderpass, &camera);
}

void game_update(Game* game){
    game->rotation += game->engine->time->time_delta * ROTATION_SPEED;

    quat_from_euler(&game->node->transform.rotation, 0, game->rotation, 0.0f);
    fw64_node_update(game->node);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_draw_static_mesh(game->renderpass, game->node->mesh, &game->node->transform);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

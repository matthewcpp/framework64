#include "game.h"
#include "assets/assets.h"

#include "framework64/n64/controller_button.h"

#define ROTATION_SPEED 90.0f

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64_camera_init(&game->camera);
    vec3_set(&game->camera.transform.position, 0.0f, 7.5f, 18.0f);
    Vec3 target = {0.0f, 0.0f, 0.0f};
    Vec3 up = {0.0f, 1.0f, 0.0f};
    fw64_transform_look_at(&game->camera.transform, &target, &up);
    fw64_camera_update_view_matrix(&game->camera);

    fw64_node_init(&game->node);
    fw64_node_set_mesh(&game->node, fw64_assets_load_mesh(engine->assets, FW64_ASSET_mesh_n64_logo, fw64_default_allocator()));
    vec3_set_all(&game->node.transform.scale, 0.1f);

    game->rotation = 0.0f;
}

void game_update(Game* game){
    game->rotation += game->engine->time->time_delta * ROTATION_SPEED;

    quat_from_euler(&game->node.transform.rotation, 0, game->rotation, 0.0f);
    fw64_node_update(&game->node);
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->camera);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->node.transform, game->node.mesh);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

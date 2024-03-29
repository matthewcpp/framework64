#include "game.h"

#include "assets/assets.h"
#include "assets/scene_Playground.h"

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    chase_camera_init(&game->chase_cam, engine);

    fw64Allocator* allocator = fw64_default_allocator();

    game->scene = fw64_assets_load_scene(game->engine->assets, FW64_ASSET_scene_Playground, allocator);

    fw64Node *node = fw64_scene_get_node(game->scene, FW64_scene_Playground_node_Player_Start);

    game->respawn_node = node;
    player_init(&game->player, game->engine, game->scene, FW64_ASSET_mesh_penguin, NULL);
    vec3_set(&game->player.node.transform.scale, 0.01f, 0.01f, 0.01f);
    game->player.node.transform.position = node->transform.position;
    fw64_node_update(&game->player.node);
    player_calculate_size(&game->player);

    game->chase_cam.target = &game->player.node.transform;


    ui_init(&game->ui, engine, &game->player);
}

void game_update(Game* game){
    player_update(&game->player);
    chase_camera_update(&game->chase_cam);

    if (game->player.node.transform.position.y < -10.0f) { // respawn player
        player_reset(&game->player, &game->respawn_node->transform.position);
    }

    ui_update(&game->ui);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->chase_cam.camera);
    fw64_scene_draw_all(game->scene, renderer);
    player_draw(&game->player);

    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);

    ui_draw(&game->ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

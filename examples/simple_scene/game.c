#include "game.h"

#include "assets.h"
#include "scene_playground.h"

void game_init(Game* level, fw64Engine* engine) {
    level->engine = engine;
    chase_camera_init(&level->chase_cam, engine);

    level->scene = fw64_scene_load(level->engine->assets, FW64_ASSET_scene_playground);

    fw64Node *node = fw64_scene_get_node(level->scene, FW64_scene_playground_node_Player_Start);

    level->respawn_node = node;
    player_init(&level->player, level->engine, level->scene, FW64_ASSET_mesh_penguin, NULL);
    vec3_set(&level->player.node.transform.scale, 0.01f, 0.01f, 0.01f);
    level->player.node.transform.position = node->transform.position;
    fw64_node_update(&level->player.node);
    player_calculate_size(&level->player);

    level->chase_cam.target = &level->player.node.transform;


    ui_init(&level->ui, engine, &level->player);
}

void game_update(Game* level){
    player_update(&level->player);
    chase_camera_update(&level->chase_cam);

    if (level->player.node.transform.position.y < -10.0f) { // respawn player
        player_reset(&level->player, &level->respawn_node->transform.position);
    }

    ui_update(&level->ui);
}

void game_draw(Game* level) {
    fw64Renderer* renderer = level->engine->renderer;

    fw64_renderer_begin(renderer, &level->chase_cam.camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_scene_draw_all(level->scene, renderer);
    player_draw(&level->player);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_NOSWAP);

    fw64_renderer_begin(renderer, &level->chase_cam.camera, FW64_RENDERER_MODE_ORTHO2D, FW64_RENDERER_FLAG_NOCLEAR);
    ui_draw(&level->ui);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

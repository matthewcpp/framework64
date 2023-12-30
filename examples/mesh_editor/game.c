#include "game.h"

#include "assets/assets.h"

#include "framework64/n64/controller_button.h"

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);
    fw64_bump_allocator_init(&game->bump_allocator, 64 * 1024);

    game->mesh = fw64_assets_load_mesh(engine->assets, FW64_ASSET_mesh_minesweeper_tile, &game->bump_allocator.interface);
    fw64_node_init(&game->node);
    fw64_node_set_mesh(&game->node, game->mesh);
}

void game_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->camera);
    fw64_renderer_draw_static_mesh(game->engine->renderer, &game->node.transform, game->node.mesh);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

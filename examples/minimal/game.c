#include "game.h"
#include "assets.h"

#include "framework64/n64/controller_button.h"

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);
}

void game_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, &game->camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

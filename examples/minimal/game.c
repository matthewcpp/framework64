#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
}

void game_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

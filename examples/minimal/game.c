#include "game.h"
#include "assets.h"

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    camera_init(&game->camera);
}

void game_update(Game* game, float time_delta){

}

void game_draw(Game* game) {
    renderer_begin(game->engine->renderer, &game->camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    renderer_end(game->engine->renderer, RENDERER_FLAG_SWAP);
}

#include "game.h"

void game_init(Game* game, System* system) {
    game->system = system;
    camera_init(&game->camera);
}

void game_update(Game* game, float time_delta){
}

void game_draw(Game* game) {
    renderer_begin(game->system->renderer, &game->camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    renderer_end(game->system->renderer, RENDERER_FLAG_SWAP);
}

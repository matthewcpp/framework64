#include "game.h"

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    level_init(&game->level, engine);
}

void game_update(Game* game){
    level_update(&game->level);
}

void game_draw(Game* game) {
    level_draw(&game->level);
}

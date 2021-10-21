#include "game.h"

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    game->level = 2;

    switch (game->level) {
        case 1:
            level1_init(&game->levels.level1, engine);
        break;

        case 2:
            level2_init(&game->levels.level2, engine);
        break;
    }

}

void game_update(Game* game){
    switch(game->level) {
        case 1:
            level1_update(&game->levels.level1);
            break;

        case 2:
            level2_update(&game->levels.level2);
            break;
    }
}

void game_draw(Game* game) {
    switch(game->level) {
        case 1:
            level1_draw(&game->levels.level1);
            break;

        case 2:
            level2_draw(&game->levels.level2);
            break;
    }
}

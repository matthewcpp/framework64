#ifndef GAME_H
#define GAME_H

#include "framework64/engine.h"

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
} Game;

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif
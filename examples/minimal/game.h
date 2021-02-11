#ifndef GAME_H
#define GAME_H

#include "framework64/system.h"

typedef struct {
    fw64System* system;
    Camera camera;
} Game;

void game_init(Game* game, fw64System* system);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif
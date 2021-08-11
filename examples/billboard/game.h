#ifndef GAME_H
#define GAME_H

#include "entity.h"

#include "framework64/util/fps_camera.h"
#include "framework64/util/quad.h"

#include "framework64/engine.h"

typedef struct {
    fw64Engine* engine;
    FpsCamera fps;
    Entity blue_cube;
    Entity nintendo_seal;
    Entity n64_logo;
} Game;

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game, float time_delta);
void game_draw(Game* game);

#endif
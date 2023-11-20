#pragma once

#include "flame.h"

#include "framework64/engine.h"
#include "framework64/node.h"

#include "framework64/util/quad.h"

#include "fps_camera/fps_camera.h"

typedef struct {
    fw64Engine* engine;
    fw64FpsCamera fps;
    fw64Node campfire;
    fw64Node ground;
    fw64Node moon;
    Flame flame;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif

#pragma once

#include "framework64/engine.h"
#include "framework64/util/terrain.h"
#include "framework64/util/fps_camera.h"

typedef struct {
    fw64Engine* engine;
    fw64FpsCamera fps_camera;
    fw64Terrain* terrain;
    fw64Font* font;
    int render_count;
    char position_text[64];
    char render_text[32];
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

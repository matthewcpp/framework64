#pragma once

#include "framework64/color.h"
#include "framework64/engine.h"

typedef struct {
    int x, y, width, height;
    float progress;
} ProgressBar;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Texture* logo_texture;
    ProgressBar progress_bar;
    fw64Font* font;
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

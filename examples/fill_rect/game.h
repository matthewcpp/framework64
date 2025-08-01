#pragma once

#include "framework64/color.h"
#include "framework64/engine.h"

typedef struct {
    int x, y, width, height;
    float progress;
} ProgressBar;

typedef enum {
    LAYER_LOGO_TEX,
    LAYER_PROGRESS_BAR,
    LAYER_PROGRESS_TEXT,
    LAYER_COUNT
} Layers;

typedef struct {
    fw64Engine* engine;
    fw64Texture* logo_texture;
    fw64Texture* fill_texture;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* sprite_batch;
    ProgressBar progress_bar;
    fw64Font* font;
} Game;

#ifdef __cplusplus
extern "C" {
#endif

void game_init(Game* game, fw64Engine* engine);
void game_fixed_update(Game* game);
void game_update(Game* game);
void game_draw(Game* game);

#ifdef __cplusplus
}
#endif

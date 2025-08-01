#pragma once

#include <framework64/engine.h>
#include <framework64/render_pass.h>
#include <framework64/sprite_batch.h>

#define FONT_COUNT 3

typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpass;
    fw64SpriteBatch* sprite_batch;

    fw64Font* fonts[FONT_COUNT];
    int active_font_index;
    Vec3 translate;
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

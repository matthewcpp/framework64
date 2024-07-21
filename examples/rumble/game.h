#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

typedef enum {
    RENDER_PASS_SCENE,
    RENDER_PASS_UI,
    RENDER_PASS_COUNT
} RenderPass;

typedef struct {
    fw64Engine* engine;

    float rumble_frequency;
    float rumble_duration;

    fw64Scene scene;

    float shake_speed;
    float shake_time;

    fw64Font* font;
    fw64Texture* buttons;
    fw64RenderPass* renderpasses[RENDER_PASS_COUNT];
    fw64SpriteBatch* spritebatch;
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

#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_arcball_camera.h"
#include "fw64_headlight.h"
#include "fw64_ui_navigation.h"

#include "ui.h"

typedef enum {
    RENDER_PASS_SCENE,
    RENDER_PASS_UI,
    RENDER_PASS_COUNT
} RenderPass;

typedef enum {
    GAME_MODE_SCENE,
    GAME_MODE_MENU
} GameMode;



typedef struct {
    fw64Engine* engine;
    fw64RenderPass* renderpasses[RENDER_PASS_COUNT];
    GameMode mode;
    Ui ui;
    fw64ArcballCamera arcball;
    fw64Scene scene;
    fw64Headlights headlights;
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

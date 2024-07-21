#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "arcball_camera/arcball_camera.h"

typedef enum {
    RENDER_PASS_SCENE,
    RENDER_PASS_UI,
    RENDER_PASS_COUNT
} RenderPasses;

typedef struct {
    fw64Engine* engine;
    fw64ArcballCamera arcball;

    fw64Font* font;
    fw64Texture* button_sprite;

    fw64Scene scene;
    int current_mesh;

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

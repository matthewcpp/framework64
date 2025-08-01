#pragma once

#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_arcball_camera.h"
#include "fw64_headlight.h"

typedef enum {
    RENDER_PASS_SCENE,
    RENDER_PASS_UI,
    RENDER_PASS_COUNT
} RenderPasses;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64ArcballCamera arcball;
    fw64Headlight headlight;

    fw64Font* font;

    fw64Scene scene;
    fw64Node* mesh_node;
    int current_mesh;

    fw64RenderPass* renderpasses[RENDER_PASS_COUNT];
    fw64SpriteBatch* spritebatch;
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

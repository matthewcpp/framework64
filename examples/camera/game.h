#pragma once

#include <framework64/engine.h>
#include <framework64/render_pass.h>
#include <framework64/scene.h>
#include <framework64/sprite_batch.h>

#include "fw64_arcball_camera.h"
#include "fw64_headlight.h"

typedef struct {
    fw64Engine* engine;
    fw64Scene* scene;
    fw64Camera camera;
    fw64ArcballCamera arcball_camera;
    fw64RenderPass* renderpass;
    fw64Headlight headlight;
} View;

typedef struct {
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
} Overlay;

typedef struct {
    fw64Engine* engine;
    fw64Font* font;
    fw64Scene scene;
    View persp_view, ortho_view;
    Overlay overlay;
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

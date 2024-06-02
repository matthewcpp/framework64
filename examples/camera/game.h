#pragma once

#include <framework64/engine.h>
#include <framework64/render_pass.h>
#include <framework64/scene.h>
#include <framework64/sprite_batch.h>

#include "arcball_camera/arcball_camera.h"

typedef struct {
    fw64ArcballCamera arcball_camera;
    fw64RenderPass* renderpass;
} View;

typedef struct {
    fw64RenderPass* renderpass;
    fw64SpriteBatch* spritebatch;
} Overlay;

void view_init(View* view, fw64Engine* engine, fw64Scene* scene, Vec2* viewport_pos, Vec2* viewport_size, fw64CameraProjectionMode projection_mode);
void view_update(View* view, float time_delta);

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

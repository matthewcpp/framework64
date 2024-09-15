#pragma once

#include "framework64/animation_controller.h"
#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"
#include "framework64/scene.h"
#include "framework64/skinned_mesh_instance.h"

#include "fw64_arcball_camera.h"
#include "fw64_headlight.h"

typedef struct {
    fw64Engine* engine;
    fw64SpriteBatch* spritebatch;
    fw64RenderPass* renderpass;
    fw64Font* font;
    fw64SkinnedMeshInstance* skinned_mesh_instance;
} Ui;

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64ArcballCamera arcball;
    fw64Headlight headlight;
    fw64Scene scene;
    fw64RenderPass* renderpass;

    int current_animation;
    float current_speed;
    Ui ui;
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

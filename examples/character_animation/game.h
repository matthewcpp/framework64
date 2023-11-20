#pragma once

#include "framework64/animation_controller.h"
#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#include "arcball_camera/arcball_camera.h"

typedef struct {
    fw64Engine* engine;
    fw64ArcballCamera arcball;

    fw64Font* consolas;
    fw64Texture* button_sprite;

    fw64Node node;
    fw64SkinnedMesh* skinned_mesh;
    fw64AnimationController animation_state;
    int current_animation;
    float current_speed;
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

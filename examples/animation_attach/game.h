#pragma once

#include "framework64/animation_controller.h"
#include "framework64/util/animated_mesh_attachment.h"
#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/node.h"

#include "common/arcball_camera.h"

typedef struct {
    fw64Engine* engine;
    fw64ArcballCamera arcball;

    fw64Font* consolas;
    fw64Texture* button_sprite;

    fw64Node character_node;
    fw64Mesh* sword_mesh;
    fw64Mesh* mace_mesh;
    fw64Mesh* current_weapon_mesh;
    fw64AnimationData* animation_data;
    fw64AnimationController animation_state;
    fw64AnimatedMeshAttachment weapon_attachment;
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

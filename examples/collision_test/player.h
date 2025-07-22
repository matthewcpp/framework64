#pragma once

#include "fw64_character.h"

#include <framework64/engine.h>
#include <framework64/scene.h>

#include "fw64_character.h"
#include "fw64_character_animation_controller.h"
#include "fw64_third_person_camera.h"
#include "fw64_third_person_input_controller.h"

typedef struct {
    fw64Engine* engine;
    fw64Node* node;
    fw64Character character;
    fw64CharacterAnimationController animation_controller;
    fw64ThirdPersonCamera third_person_cam;
    fw64ThirdPersonInputController third_person_input;
    int run_character_update;
    Vec3 initial_pos;

    // --- temp ----
    fw64Scene* scene;
} Player;

void player_init(Player* player, fw64Engine* engine, fw64CharacterEnvironment* env, fw64Scene* scene, fw64Node* node, fw64Camera* camera, fw64Allocator* allocator);
void player_update(Player* player);
void player_fixed_update(Player* player);

#pragma once

#include "fw64_character_animation_controller.h"
#include "fw64_character.h"
#include "fw64_third_person_camera.h"

#include <framework64/camera.h>
#include <framework64/input.h>

typedef struct {
    fw64Input* input;
    fw64Character* character;
    fw64Node* node;
    fw64ThirdPersonCamera* cam;
    fw64CharacterAnimationController* anim;

    int port;
    float stick_threshold;
    float cam_rotation_speed;
    float cam_dist_speed;
} fw64ThirdPersonInputController;

void fw64_third_person_input_controller_init(fw64ThirdPersonInputController* controller, fw64Input* input, fw64Character* character, fw64Node* node, fw64ThirdPersonCamera* cam, fw64CharacterAnimationController* anim, int port);
void fw64_third_person_input_controller_update(fw64ThirdPersonInputController* controller, float time_delta);

#pragma once

#include "fw64_character.h"

#include <framework64/camera.h>
#include <framework64/input.h>

#define FW64_THIRD_PERSON_INPUT_STICK_THRESHOLD 0.1f

typedef struct {
    fw64Input* input;
    fw64Character* character;
    fw64Node* node;
    fw64Camera* camera;

    int port;
    float stick_threshold;
} fw64ThirdPersonInputController;

void fw64_third_person_input_controller_init(fw64ThirdPersonInputController* controller, fw64Input* input, fw64Character* character, fw64Node* node, fw64Camera* camera, int port);
void fw64_third_person_input_controller_update(fw64ThirdPersonInputController* controller);

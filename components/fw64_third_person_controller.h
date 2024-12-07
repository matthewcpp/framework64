#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/scene.h"

#include "fw64_character_controller.h"

typedef struct {
    fw64CharacterController base;
    fw64Engine* engine;
    fw64Camera* camera;
    int process_input;
    int controller_num;
} fw64ThirdPersonController;

void fw64_third_person_controller_init(fw64ThirdPersonController* controller, fw64Engine* engine, fw64Scene* scene, fw64Collider* collider);
void fw64_third_person_controller_fixed_update(fw64ThirdPersonController* controller);
void fw64_third_person_controller_update(fw64ThirdPersonController* controller);

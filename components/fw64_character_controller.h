#pragma once

#include "framework64/scene.h"

typedef struct {
    fw64Scene* scene;
    Vec3 velocity;
    fw64Collider* collider;
} fw64CharacterController;

void fw64_character_controller_init(fw64CharacterController* controller, fw64Scene* scene, fw64Collider* collider);
void fw64_character_fixed_update(fw64CharacterController* controller, float time_delta);

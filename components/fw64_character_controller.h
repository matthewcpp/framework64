#pragma once

#include "framework64/scene.h"

typedef enum {
    FW64_CHARACTER_CONTROLLER_STATE_GROUNDED,
    FW64_CHARACTER_CONTROLLER_STATE_FALLING
} fw64CharacterControllerState;

typedef enum {
    FW64_CHARACTER_CONTROLLER_FLAG_NONE         = 0,
    FW64_CHARACTER_CONTROLLER_FLAG_ACCELERATE   = 1 << 0,
    FW64_CHARACTER_CONTROLLER_FLAG_DECELERATE   = 1 << 1
} fw64CharacterControllerFlags;

typedef struct {
    fw64Scene* scene;
    Vec3 velocity;
    fw64Collider* collider;

    Vec3 previous_position;

    fw64CharacterControllerState state;
    fw64CharacterControllerFlags flags;

    float jump_impulse;
    float air_velocity;
    float gravity;

    float height;
    float radius;

    float speed;
    float acceleration;
    float deceleration;
    float max_speed;

    float rotation;
    float rotation_speed;
} fw64CharacterController;

void fw64_character_controller_init(fw64CharacterController* controller, fw64Scene* scene, fw64Collider* collider);
void fw64_character_controller_fixed_update(fw64CharacterController* controller, float time_delta);
void fw64_character_controller_reset(fw64CharacterController* controller);

// TODO: these should go away or be refactored in some form
void fw64_character_controller_accelerate(fw64CharacterController* controller);
void fw64_character_controller_decelerate(fw64CharacterController* controller);
void fw64_character_controller_jump(fw64CharacterController* controller);

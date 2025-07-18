#pragma once

#include "framework64/scene.h"

/** gravity constant in meters per second */
#define Fw64_CHARACTER_ENV_DEFAULT_GRAVITY -9.81f
#define FW64_CHARACTER_ENV_MAX_FALL_SPEED -15.0
#define FW64_CHARACTER_ENV_MAX_SUBSTEPS 5
#define FW64_CHARACTER_ENV_HORIZ_MOVE_THRESHOLD 0.01

// #define FW64_CHARACTER_DEFAULT_JUMP_SPEED 4.4
#define FW64_CHARACTER_DEFAULT_JUMP_SPEED 6.4
#define FW64_CHARACTER_DEFAULT_MAX_SPEED 10.0
#define FW64_CHARACTER_DEFAULT_GRAVITY_SCALE 1.5f
#define FW64_CHARACTER_DEFAULT_JUMP_FALL_GRAVITY_SCALE 2.0f

typedef struct {
    Vec3 gravity;
    int max_substeps;

    /** The maximum rate that characters can fall in meters / sec */
    float max_fall_speed;

    /** When grounded, the minimum amount of distance a character must attempt to move horizontally.
      * This is intended to prevent jittering from floating point rounding 
    */
    float horizontal_move_threshold;
} fw64CharacterEnvironment;

void fw64_character_envionment_init(fw64CharacterEnvironment* env);

typedef enum {
    FW64_CHARACTER_STATE_ON_GROUND,
    FW64_CHARACTER_STATE_IN_AIR
} fw64CharacterState;

typedef struct {
    fw64CharacterEnvironment* environment;
    fw64Scene* scene;

    Vec3 velocity;
    fw64CharacterState previous_state, state;
    /** This is interpreted as the bottom center of the character. */
    Vec3 previous_position, position;

    /* The size of the character in each dimension*/
    Vec3 size;

    /* The maximum distance a character can move in one second*/
    float max_speed;

    float jump_speed;
    float gravity_scale;
    float jump_fall_gravity_scale;

    Vec3 attempt_to_move;
    int attempt_to_jump;

} fw64Character;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Scene* scene);
void fw64_character_fixed_update(fw64Character* character, float time_delta);

void fw64_character_reset_position(fw64Character* character, const Vec3* position);

#define fw64_character_is_on_ground(character) ((character)->state == FW64_CHARACTER_STATE_ON_GROUND)
#define fw64_character_is_in_air(character) ((character)->state == FW64_CHARACTER_STATE_IN_AIR)
#ifdef __cplusplus
}
#endif

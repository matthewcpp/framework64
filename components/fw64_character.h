#pragma once

#include "framework64/scene.h"

/** gravity constant in meters per second */
#define Fw64_CHARACTER_ENV_DEFAULT_GRAVITY -9.81f
#define FW64_CHARACTER_MAX_FALL_SPEED -15.0
#define FW64_CHARACTER_ENV_MAX_SUBSTEPS 5


#define FW64_CHARACTER_DEFAULT_JUMP_SPEED 4.4
#define FW64_CHARACTER_DEFAULT_MAX_SPEED 7.5

typedef struct {
    Vec3 gravity;
    int max_substeps;
    float max_fall_speed;
} fw64CharacterEnvironment;

void fw64_character_envionment_init(fw64CharacterEnvironment* env);

typedef enum {
    FW64_CHARACTER_ON_GROUND,
    FW64_CHARACTER_IN_AIR
} fw64CharacterState;

typedef struct {
    fw64CharacterEnvironment* environment;
    Vec3 velocity;
    fw64CharacterState state;
    fw64Scene* scene;

    /** This is interpreted as the bottom center of the character. */
    Vec3 previous_position, position;

    /* The size of the character in each dimension*/
    Vec3 size;

    /* The maximum distance a character can move in one second*/
    float max_speed;

    Vec3 attempt_to_move;
    int attempt_to_jump;
    float jump_speed;
} fw64Character;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Scene* scene);
void fw64_character_fixed_update(fw64Character* character, float time_delta);

void vw64_character_reset_position(fw64Character* character, const Vec3* position);

#define fw64_character_is_on_ground(character) ((character)->state == FW64_CHARACTER_ON_GROUND)

#ifdef __cplusplus
}
#endif

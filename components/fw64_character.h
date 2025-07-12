#pragma once

#include "framework64/scene.h"

/** gravity constant in meters per second */
#define Fw64_CHARACTER_ENV_DEFAULT_GRAVITY -9.81f

typedef struct {
    Vec3 gravity;
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

    /* The height that a player can "step up "*/
    float step_height;
} fw64Character;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Scene* scene);
void fw64_character_fixed_update(fw64Character* character, float time_delta);

#define fw64_character_is_on_ground(character) ((character)->state == FW64_CHARACTER_ON_GROUND)

#ifdef __cplusplus
}
#endif

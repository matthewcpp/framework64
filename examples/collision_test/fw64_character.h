#pragma once

#include "framework64/scene.h"

#define Fw64_CHARACTER_ENV_DEFAULT_GRAVITY -9.81

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
    Vec3 position;
    Vec3 velocity;
    fw64CharacterState state;
    fw64Scene* scene;
} fw64Character;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Scene* scene);
void fw64_character_update(fw64Character* character, float time_delta);

#define fw64_character_is_on_ground(character) ((character)->state == FW64_CHARACTER_ON_GROUND)

#ifdef __cplusplus
}
#endif

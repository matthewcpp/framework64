#include "fw64_character.h"

void fw64_character_envionment_init(fw64CharacterEnvironment* env) {
    vec3_set(&env->gravity, 0.0f, Fw64_CHARACTER_ENV_DEFAULT_GRAVITY, 0.0f);
}

void fw64_character_init(fw64Character* character, fw64CharacterEnvironment* env, fw64Scene* scene) {
    character->environment = env;
    character->scene = scene;

    character->state = FW64_CHARACTER_ON_GROUND;
    vec3_set_zero(&character->velocity);
}

void fw64_character_update(fw64Character* character, float time_delta) {
    vec3_add_and_scale(&character->position, &character->environment->gravity, time_delta, &character->position);
    vec3_add_and_scale(&character->position, &character->velocity, time_delta, &character->position);

    if (character->position.y < 0.0f) {
        character->position.y = 0.0f;
        character->state = FW64_CHARACTER_ON_GROUND;
    } else {
        character->state = FW64_CHARACTER_IN_AIR;
    }
}

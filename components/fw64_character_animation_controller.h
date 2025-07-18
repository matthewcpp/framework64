#pragma once

#include <framework64/animation_controller.h>
#include <framework64/skinned_mesh_instance.h>

#include "fw64_character.h"

typedef struct {
    fw64AnimationId idle;
    fw64AnimationId run;
    fw64AnimationId jump_up;
    fw64AnimationId jump_fall;
    fw64AnimationId jump_land;
} fw64CharacterAnimationIds;

typedef struct {
    fw64CharacterAnimationIds animations;
    fw64Character* character;
    fw64SkinnedMeshInstance* skinned_mesh_instance;
} fw64CharacterAnimationController;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_character_animation_controller_init(fw64CharacterAnimationController* controller, fw64Character* character, fw64SkinnedMeshInstance* skinned_mesh_instance, const fw64CharacterAnimationIds* animations);
void fw64_character_animation_controller_update(fw64CharacterAnimationController* controller, float time_delta);

#ifdef __cplusplus
}
#endif

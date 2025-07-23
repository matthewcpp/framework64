#pragma once

#include <framework64/animation_controller.h>
#include <framework64/skinned_mesh_instance.h>

#include "fw64_character.h"

typedef struct fw64CharacterAnimationController fw64CharacterAnimationController;

typedef void (*fw64CharacterAnimationControllerStateFunc)(fw64CharacterAnimationController* controller, fw64AnimationId prev, void* arg);

typedef struct {
    fw64AnimationId idle;
    fw64AnimationId run;
    fw64AnimationId jump_up;
    fw64AnimationId jump_fall;
    fw64AnimationId jump_land;
    fw64AnimationId primary_action;
} fw64CharacterAnimationIds;

struct fw64CharacterAnimationController{
    fw64CharacterAnimationIds animations;
    fw64Character* character;
    fw64SkinnedMeshInstance* skinned_mesh_instance;
    fw64AnimationId animation_state;
    fw64CharacterAnimationControllerStateFunc transition_func;
    void* transition_arg;
    float fall_start_height;

    /** The amount of distance a character needs to fall before triggering an animations tate change */
    float fall_transition_threshold;
    uint32_t flags;
};

#ifdef __cplusplus
extern "C" {
#endif

void fw64_character_animation_controller_init(fw64CharacterAnimationController* controller, fw64Character* character, fw64SkinnedMeshInstance* skinned_mesh_instance, const fw64CharacterAnimationIds* animations);
void fw64_character_animation_controller_update(fw64CharacterAnimationController* controller, float time_delta);
void fw64_character_animation_controller_start_primary_action(fw64CharacterAnimationController* controller);
void fw64_character_animation_controller_set_transition_func(fw64CharacterAnimationController* controller, fw64CharacterAnimationControllerStateFunc transition_func, void* transition_arg);

#ifdef __cplusplus
}
#endif

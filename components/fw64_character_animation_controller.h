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
    fw64AnimationId ledge_hang_idle;
    fw64AnimationId ledge_climb;
} fw64CharacterAnimationIds;

struct fw64CharacterAnimationController{
    fw64CharacterAnimationIds animations;
    fw64Character* character;
    fw64SkinnedMeshInstance* skinned_mesh_instance;
    fw64AnimationId animation_state;
    fw64CharacterAnimationControllerStateFunc transition_func;
    void* transition_arg;
    float fall_start_height;

    /** This id will be used to retrieve a joint when a world space point is needed relative to the character's feet. 
     *  The default value is set to 0, however this will most likely give undesireable results.
     *  Ensure that it is set to the proper value for the rig that you are using.
    */
    int foot_reference_joint_index;

    /** The amount of distance a character needs to fall before triggering an animations tate change */
    float fall_transition_threshold;
    uint32_t flags;
};

#ifdef __cplusplus
extern "C" {
#endif

void fw64_character_animation_controller_init(fw64CharacterAnimationController* controller, fw64Character* character, fw64SkinnedMeshInstance* skinned_mesh_instance, const fw64CharacterAnimationIds* animations);
void fw64_character_animation_controller_update(fw64CharacterAnimationController* controller, float time_delta);
void fw64_character_animation_controller_set_transition_func(fw64CharacterAnimationController* controller, fw64CharacterAnimationControllerStateFunc transition_func, void* transition_arg);

void fw64_character_animation_controller_start_primary_action(fw64CharacterAnimationController* controller);

#define fw64_character_animation_controller_state_is_idle(controller) ((controller)->animation_state == (controller)->animations.idle)
#define fw64_character_animation_controller_state_is_running(controller) ((controller)->animation_state == (controller)->animations.run)
#define fw64_character_animation_controller_state_is_primary_action(controller) ((controller)->animation_state == (controller)->animations.primary_action)
#define fw64_character_animation_controller_state_is_ledge_hanging(controller) ((controller)->animation_state == (controller)->animations.ledge_hang_idle)
#ifdef __cplusplus
}
#endif

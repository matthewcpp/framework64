#include "fw64_character_animation_controller.h"

#include <framework64/log.h>
#include <framework64/matrix.h>

typedef enum {
    FW64_CHARACTER_ANIMATION_CONTROLLER_FLAG_NONE = 0,
    FW64_CHARACTER_ANIMATION_CONTROLLER_FLAG_RUNNING_IN_AIR = 1
}fw64CharacterAnimationControllerFlags;

#define FW64_CHARACTER_ANIMATION_CONTROLLER_DEFAULT_FALL_TRANSITION_THRESHOLD 0.25

void fw64_character_animation_controller_init(fw64CharacterAnimationController* controller, fw64Character* character, fw64SkinnedMeshInstance* skinned_mesh_instance, const fw64CharacterAnimationIds* animations) {
    controller->character = character;
    controller->skinned_mesh_instance = skinned_mesh_instance;
    controller->animations = *animations;
    controller->animation_state = FW64_INVALID_ANIMATION_ID;
    controller->flags = FW64_CHARACTER_ANIMATION_CONTROLLER_FLAG_NONE;
    controller->fall_transition_threshold = FW64_CHARACTER_ANIMATION_CONTROLLER_DEFAULT_FALL_TRANSITION_THRESHOLD;
    fw64_character_animation_controller_set_transition_func(controller, NULL, NULL);
}

static void fw64_character_animation_controller_set_animation(fw64CharacterAnimationController* controller, fw64AnimationId next_animation, int loop) {
    fw64AnimationController* anim_controller = &controller->skinned_mesh_instance->controller;
    fw64AnimationId previous_state = controller->animation_state;

    controller->animation_state = next_animation;
    fw64_animation_controller_set_animation(anim_controller, controller->animation_state);
    anim_controller->loop = loop;
    fw64_animation_controller_play(anim_controller);

    if (controller->transition_func) {
        controller->transition_func(controller, previous_state, controller->transition_arg);
    }
}

static void fw64_character_animation_controller_update_invalid_state(fw64CharacterAnimationController* controller) {
    if (fw64_character_is_falling(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_fall, 0);
    } else if (fw64_character_is_on_ground(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.idle, 1);
    }
}

static void fw64_character_animation_controller_update_jump_up_state(fw64CharacterAnimationController* controller) {
    // did the character reach the jump apex?
    if (controller->character->velocity.y < 0.0f) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_fall, 0);
    } else if (fw64_character_is_on_ground(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_land, 0);
    } else if (fw64_character_is_hanging_on_ledge(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.ledge_hang_idle, 1);
    }
}

static void fw64_character_animation_controller_update_falling_state(fw64CharacterAnimationController* controller) {
    // did the character land?
    if (fw64_character_is_on_ground(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_land, 0);
    } else if (fw64_character_is_hanging_on_ledge(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.ledge_hang_idle, 1);
    }
}

static void fw64_character_animation_controller_update_landing_state(fw64CharacterAnimationController* controller) {
    // did we finish recovering from the land position?
    if (fw64_animation_controller_is_stopped(&controller->skinned_mesh_instance->controller)) {
        fw64AnimationId next_animation = fw64_character_is_moving_horizontally(controller->character) ? controller->animations.run : controller->animations.idle;
        fw64_character_animation_controller_set_animation(controller, next_animation, 1);
    }
}

static void fw64_character_animation_controller_update_running_state(fw64CharacterAnimationController* controller) {
    if (!fw64_character_is_moving_horizontally(controller->character)) {
        controller->flags &= ~FW64_CHARACTER_ANIMATION_CONTROLLER_FLAG_RUNNING_IN_AIR;
        fw64_character_animation_controller_set_animation(controller, controller->animations.idle, 1);
    } else if (controller->character->velocity.y > 0.0f) {
        controller->flags &= ~FW64_CHARACTER_ANIMATION_CONTROLLER_FLAG_RUNNING_IN_AIR;
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_up, 0);
    } if (fw64_character_is_falling(controller->character)) {
        // we want to allow a grace perid before we trigger a new animation
        if ((controller->flags & FW64_CHARACTER_ANIMATION_CONTROLLER_FLAG_RUNNING_IN_AIR)) {
            const float fall_height = controller->fall_start_height - controller->character->position.y;
            if (fall_height >controller->fall_transition_threshold) {
                fw64_character_animation_controller_set_animation(controller, controller->animations.jump_fall, 0);
                controller->flags &= ~FW64_CHARACTER_ANIMATION_CONTROLLER_FLAG_RUNNING_IN_AIR;
            }
        } else { // player ran off the ground, record the position to track the fall length
            controller->fall_start_height = controller->character->position.y;
            controller->flags |= FW64_CHARACTER_ANIMATION_CONTROLLER_FLAG_RUNNING_IN_AIR;
        }
    } else {
        controller->flags &= ~FW64_CHARACTER_ANIMATION_CONTROLLER_FLAG_RUNNING_IN_AIR;
    }
}

static void fw64_character_animation_controller_update_idle_state(fw64CharacterAnimationController* controller) {
    if (fw64_character_is_moving_horizontally(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.run, 1);
    } else if (controller->character->velocity.y > 0.0f) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_up, 0);
    }
}

static void fw64_character_animation_controller_update_primary_action(fw64CharacterAnimationController* controller) {
    if (fw64_animation_controller_is_stopped(&controller->skinned_mesh_instance->controller)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.idle, 1);
    }
}

static void fw64_character_animation_controller_update_ledge_hang_idle(fw64CharacterAnimationController* controller) {
    if (fw64_character_is_climbing_up_ledge(controller->character)) { // player started to climb up the ledge
        fw64_character_animation_controller_set_animation(controller, controller->animations.ledge_climb, 0);
    } else if (!fw64_character_is_hanging_on_ledge(controller->character)) { // player dropped of ledge
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_fall, 0);
    }
}

static void fw64_character_animation_controller_climbing_up_ledge(fw64CharacterAnimationController* controller) {
    fw64AnimationController* anim_controller = &controller->skinned_mesh_instance->controller;

    // if the animation is finished we want to transition to idle and adjust the character
    // We will determine the final end point based on the animation
    // TODO: perhaps we would rather drive this via code?
    if (fw64_animation_controller_is_stopped(anim_controller)) {
        Vec3 ref_pos = vec3_zero();
        const fw64Transform* root_transform = fw64_animation_controller_get_joint_transform(anim_controller, controller->foot_reference_joint_index);
        matrix_transform_vec3(root_transform->world_matrix, &ref_pos);
        fw64_character_finish_climbing_up_ledge(controller->character, &ref_pos);
        fw64_character_animation_controller_set_animation(controller, controller->animations.idle, 1);
    } 
}

void fw64_character_animation_controller_update(fw64CharacterAnimationController* controller, float time_delta) {
    if (controller->animation_state == FW64_INVALID_ANIMATION_ID) {
        fw64_character_animation_controller_update_invalid_state(controller);
    } else if (controller->animation_state == controller->animations.jump_fall) {
        fw64_character_animation_controller_update_falling_state(controller);
    } else if (controller->animation_state == controller->animations.jump_land) {
        fw64_character_animation_controller_update_landing_state(controller);
    } else if (controller->animation_state == controller->animations.run) {
        fw64_character_animation_controller_update_running_state(controller);
    } else if (controller->animation_state == controller->animations.idle) {
        fw64_character_animation_controller_update_idle_state(controller);
    } else if (controller->animation_state == controller->animations.jump_up) {
        fw64_character_animation_controller_update_jump_up_state(controller);
    } else if (controller->animation_state == controller->animations.primary_action) {
        fw64_character_animation_controller_update_primary_action(controller);
    } else if (controller->animation_state == controller->animations.ledge_hang_idle) {
        fw64_character_animation_controller_update_ledge_hang_idle(controller);
    } else if (controller->animation_state == controller->animations.ledge_climb) {
        fw64_character_animation_controller_climbing_up_ledge(controller);
    }

    fw64_skinned_mesh_instance_update(controller->skinned_mesh_instance, time_delta);
}

void fw64_character_animation_controller_start_primary_action(fw64CharacterAnimationController* controller) {
    fw64_character_animation_controller_set_animation(controller, controller->animations.primary_action, 0);
}

void fw64_character_animation_controller_set_transition_func(fw64CharacterAnimationController* controller, fw64CharacterAnimationControllerStateFunc transition_func, void* transition_arg) {
    controller->transition_func = transition_func;
    controller->transition_arg = transition_arg;
}

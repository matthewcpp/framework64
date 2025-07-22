#include "fw64_character_animation_controller.h"


void fw64_character_animation_controller_init(fw64CharacterAnimationController* controller, fw64Character* character, fw64SkinnedMeshInstance* skinned_mesh_instance, const fw64CharacterAnimationIds* animations) {
    controller->character = character;
    controller->skinned_mesh_instance = skinned_mesh_instance;
    controller->animations = *animations;
    controller->animation_state = FW64_INVALID_ANIMATION_ID;
}

static void fw64_character_animation_controller_set_animation(fw64CharacterAnimationController* controller, fw64AnimationId next_animation, int loop) {
    fw64AnimationController* anim_controller = &controller->skinned_mesh_instance->controller;

    controller->animation_state = next_animation;
    fw64_animation_controller_set_animation(anim_controller, controller->animation_state);
    anim_controller->loop = loop;
    fw64_animation_controller_play(anim_controller);
}

static void fw64_character_animation_controller_update_invalid_state(fw64CharacterAnimationController* controller) {
    // character is falling
    if (controller->character->velocity.y < 0.0f) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_fall, 0);
    }
}

static void fw64_character_animation_controller_update_jump_up_state(fw64CharacterAnimationController* controller) {
    // did the character reach the jump apex?
    if (controller->character->velocity.y < 0.0f) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_fall, 0);
    } else if (fw64_character_is_on_ground(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_land, 0);
    }
}

static void fw64_character_animation_controller_update_falling_state(fw64CharacterAnimationController* controller) {
    // did the character land?
    if (fw64_character_is_on_ground(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_land, 0);
    }
}

static void fw64_character_animation_controller_update_landing_state(fw64CharacterAnimationController* controller) {
    fw64AnimationController* anim_controller = &controller->skinned_mesh_instance->controller;

    // did we finish recovering from the land position?
    if (fw64_animation_controller_is_stopped(anim_controller)) {
        fw64AnimationId next_animation = fw64_character_is_moving_horizontally(controller->character) ? controller->animations.run : controller->animations.idle;
        fw64_character_animation_controller_set_animation(controller, next_animation, 1);
    }
}

static void fw64_character_animation_controller_update_running_state(fw64CharacterAnimationController* controller) {
    if (!fw64_character_is_moving_horizontally(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.idle, 1);
    } else if (controller->character->velocity.y > 0.0f) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_up, 0);
    }
}

static void fw64_character_animation_controller_update_idle_state(fw64CharacterAnimationController* controller) {
    if (fw64_character_is_moving_horizontally(controller->character)) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.run, 1);
    } else if (controller->character->velocity.y > 0.0f) {
        fw64_character_animation_controller_set_animation(controller, controller->animations.jump_up, 0);
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
    }

    fw64_skinned_mesh_instance_update(controller->skinned_mesh_instance, time_delta);

}

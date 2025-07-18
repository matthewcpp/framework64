#include "fw64_character_animation_controller.h"


void fw64_character_animation_controller_init(fw64CharacterAnimationController* controller, fw64Character* character, fw64SkinnedMeshInstance* skinned_mesh_instance, const fw64CharacterAnimationIds* animations) {
    controller->character = character;
    controller->skinned_mesh_instance = skinned_mesh_instance;
    controller->animations = *animations;
}

void fw64_character_animation_controller_update(fw64CharacterAnimationController* controller, float time_delta) {
    fw64_skinned_mesh_instance_update(controller->skinned_mesh_instance, time_delta);
}

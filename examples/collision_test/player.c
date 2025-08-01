#include "player.h"

#include "assets/assets.h"
#include "assets/character_animation.h"

#include "framework64/controller_mapping/n64.h"

static const fw64CharacterAnimationIds ids = {
    character_animation_Idle,
    character_animation_RunForward,
    character_animation_RunJumpUp,
    character_animation_RunJumpDown,
    character_animation_RunJumpLand,
    character_animation_Punch,
    character_animation_HangIdle,
    character_animation_LedgeClimbUp
};

static void player_on_animation_state_changed(fw64CharacterAnimationController* controller, fw64AnimationId prev, void* arg);

void player_init(Player* player, fw64Engine* engine, fw64CharacterEnvironment* env, fw64Scene* scene, fw64Node* node, fw64Node* camera_node, fw64Allocator* allocator) {
    player->engine = engine;
    player->node = node;
    player->scene = scene;
    player->character_control_enabled = 1;

    player->initial_pos = player->node->transform.position;

    fw64SkinnedMesh* skinned_mesh = fw64_assets_load_skinned_mesh(engine->assets, FW64_ASSET_skinnedmesh_character, allocator);
    fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_create_skinned_mesh_instance(scene, player->node, skinned_mesh, character_animation_RunForward);

    fw64_camera_init(&player->camera, camera_node, fw64_displays_get_primary(engine->displays));

    // Initialize all of the 3rd person character controller components here
    fw64_character_init(&player->character, env, player->node, scene);
    fw64_character_animation_controller_init(&player->animation_controller, &player->character, skinned_mesh_instance, &ids);
    fw64_third_person_camera_init(&player->third_person_cam, &node->transform, &player->camera);
    fw64_third_person_input_controller_init(&player->third_person_input, engine->input, &player->character, node, &player->third_person_cam, &player->animation_controller, 0);

    // Configure 3rd person character components
    vec3_set(&player->character.size, 0.2f, 1.75f, 0.2f);
    player->character.head_height = 1.5f;
    player->character.hang_vertical_offset = -0.70f;
    fw64_character_set_position(&player->character, &player->node->transform.position);
    vec3_set(&player->third_person_cam.target_offset, 0.0f, player->character.size.y, 0.0f); // focus on character's head
    fw64_character_animation_controller_set_transition_func(&player->animation_controller, player_on_animation_state_changed, NULL);

    player->animation_controller.foot_reference_joint_index = character_joint_Foot_L;
}

void player_set_character_control_enabled(Player* player, int enabled) {
    player->character_control_enabled = enabled;

    if (enabled) {
        fw64_character_enable(&player->character);
    } else {
        fw64_character_disable(&player->character);
    }
}

void player_update(Player* player) {
    vec3_lerp(&player->character.previous_position, &player->character.position, player->engine->time->accumulator_progress, &player->node->transform.position);
    fw64_node_update(player->node);

    if (player->character_control_enabled) {
        fw64_third_person_input_controller_update(&player->third_person_input, player->engine->time->time_delta);
        fw64_third_person_camera_update(&player->third_person_cam);
        fw64_character_animation_controller_update(&player->animation_controller, player->engine->time->time_delta);
    }

}

void player_fixed_update(Player* player) {
    fw64_character_fixed_update(&player->character, player->engine->time->fixed_time_delta);
}

void player_on_animation_state_changed(fw64CharacterAnimationController* controller, fw64AnimationId prev, void* arg) {
    (void)prev;
    (void)arg;
    switch (controller->animation_state) {
        case character_animation_Idle:
        case character_animation_HangIdle:
            controller->skinned_mesh_instance->controller.speed = 0.2f;
            break;

        case character_animation_Punch:
            controller->skinned_mesh_instance->controller.speed = 2.0f;
            break;
    
        default:
            controller->skinned_mesh_instance->controller.speed = 1.0f;
            break;
    }
}

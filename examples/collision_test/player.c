#include "player.h"

#include "assets/assets.h"
#include "assets/character_animation.h"

#include "framework64/controller_mapping/n64.h"

static const fw64CharacterAnimationIds ids = {
    character_animation_Idle,
    character_animation_RunForward,
    character_animation_RunJumpUp,
    character_animation_RunJumpDown,
    character_animation_RunJumpLand
};

void player_init(Player* player, fw64Engine* engine, fw64CharacterEnvironment* env, fw64Scene* scene, fw64Node* node, fw64Allocator* allocator) {
    player->engine = engine;
    player->node = node;
    player->scene = scene;
    fw64_character_init(&player->character, env, scene);

    player->run_character_update = 1;
    player->initial_pos = player->node->transform.position;

    // TODO...better way to determine this size?
    vec3_set(&player->character.size, 1.0f, 5.0f, 0.0f);
    fw64_character_reset_position(&player->character, &player->node->transform.position);


    player->node2 = fw64_scene_create_node_with_parent(scene, fw64_scene_get_node(scene, 0));
    vec3_set_all(&player->node2->transform.scale, 0.020);
    quat_from_euler(&player->node2->transform.rotation, 0, 180.0f, 0.0f);
    fw64_node_update(player->node2);
    fw64SkinnedMesh* skinned_mesh = fw64_assets_load_skinned_mesh(engine->assets, FW64_ASSET_skinnedmesh_character, allocator);
    fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_create_skinned_mesh_instance(scene, player->node2, skinned_mesh, character_animation_RunForward);

    fw64_animation_controller_play(&skinned_mesh_instance->controller);
    fw64_character_animation_controller_init(&player->animation_controller, &player->character, skinned_mesh_instance, &ids);
}

void player_update(Player* player) {
    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        player->run_character_update = !player->run_character_update;
    }

    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_A)) {
        player->character.attempt_to_jump = 1;
    }

    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z)) {
        fw64_character_reset_position(&player->character, &player->initial_pos);
        player->animation_controller.animation_state = FW64_INVALID_ANIMATION_ID; // temp
    }

    // TODO: need to actually figure this out based on camera
    Vec2 stick;
    fw64_input_controller_stick(player->engine->input, 0, &stick);
    vec3_set(&player->character.attempt_to_move, stick.x, 0.0f, -stick.y);

    vec3_lerp(&player->character.previous_position, &player->character.position, player->engine->time->accumulator_progress, &player->node->transform.position);
    fw64_node_update(player->node);

    player->node2->transform.position = player->node->transform.position;
    fw64_node_update(player->node2);

    fw64_character_animation_controller_update(&player->animation_controller, player->engine->time->time_delta);
}

void player_fixed_update(Player* player) {
    if (player->run_character_update) {
        fw64_character_fixed_update(&player->character, player->engine->time->fixed_time_delta);
    }
}

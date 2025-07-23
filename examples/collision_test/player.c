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
    character_animation_Punch
};

static void player_on_animation_state_changed(fw64CharacterAnimationController* controller, fw64AnimationId prev, void* arg);

void player_init(Player* player, fw64Engine* engine, fw64CharacterEnvironment* env, fw64Scene* scene, fw64Node* node, fw64Camera* camera, fw64Allocator* allocator) {
    player->engine = engine;
    player->node = node;
    player->scene = scene;
    

    player->run_character_update = 1;
    player->initial_pos = player->node->transform.position;

    const Vec3 character_size = node->transform.scale;
    fw64SkinnedMesh* skinned_mesh = fw64_assets_load_skinned_mesh(engine->assets, FW64_ASSET_skinnedmesh_character, allocator);
    fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_create_skinned_mesh_instance(scene, player->node, skinned_mesh, character_animation_RunForward);

    // The player height is determined by examining the initial scale of the player node
    // When attaching the skinned mesh to the player node, the scale needs to be adjusted to match the character's size;
    // Note this assumes that the character model's min.y is 0.0f
    const Box mesh_bounding = fw64_mesh_get_bounding_box(skinned_mesh->mesh);
    float node_scale = character_size.y / mesh_bounding.max.y;
    vec3_set_all(&player->node->transform.scale, node_scale);
    fw64_node_update(player->node);

    // Initialize all of the 3rd person character controller components here
    fw64_character_init(&player->character, env, scene);
    fw64_animation_controller_play(&skinned_mesh_instance->controller);
    fw64_character_animation_controller_init(&player->animation_controller, &player->character, skinned_mesh_instance, &ids);
    fw64_third_person_camera_init(&player->third_person_cam, &node->transform, camera);
    fw64_third_person_input_controller_init(&player->third_person_input, engine->input, &player->character, node, &player->third_person_cam, &player->animation_controller, 0);

    // Configure 3rd person character components
    player->character.size = character_size;
    fw64_character_reset_position(&player->character, &player->node->transform.position);
    vec3_set(&player->third_person_cam.target_offset, 0.0, character_size.y, 0.0f); // focus on character's head
    fw64_character_animation_controller_set_transition_func(&player->animation_controller, player_on_animation_state_changed, NULL);
}

void player_update(Player* player) {
    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        player->run_character_update = !player->run_character_update;
    }

    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z)) {
        fw64_character_reset_position(&player->character, &player->initial_pos);
        player->animation_controller.animation_state = FW64_INVALID_ANIMATION_ID; // temp
    }

    fw64_third_person_input_controller_update(&player->third_person_input, player->engine->time->time_delta);
    fw64_third_person_camera_update(&player->third_person_cam);

    vec3_lerp(&player->character.previous_position, &player->character.position, player->engine->time->accumulator_progress, &player->node->transform.position);
    fw64_node_update(player->node);

    fw64_character_animation_controller_update(&player->animation_controller, player->engine->time->time_delta);
}

void player_fixed_update(Player* player) {
    if (player->run_character_update) {
        fw64_character_fixed_update(&player->character, player->engine->time->fixed_time_delta);
    }
}

void player_on_animation_state_changed(fw64CharacterAnimationController* controller, fw64AnimationId prev, void* arg) {
    (void)prev;
    (void)arg;
    switch (controller->animation_state) {
        case character_animation_Idle:
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

#include "player.h"

#include "framework64/controller_mapping/n64.h"

static void process_input(Player* player);

void player_init(Player* player, fw64Engine* engine, fw64Scene* scene, int mesh_index, Vec3* position) {
    player->engine = engine;
    player->scene = scene;

    player->process_input = 1;
    player->controller_num = 0;

    player->node = fw64_scene_create_node(scene);
    fw64Mesh* mesh = fw64_scene_load_mesh_asset(scene, mesh_index);
    fw64_scene_create_mesh_instance(scene, player->node, mesh);
    Box bounding = fw64_mesh_get_bounding_box(mesh);
    fw64Collider* collider = fw64_scene_create_box_collider(scene, player->node, &bounding);

    fw64_character_controller_init(&player->character_controller, scene, collider);

    player_reset(player, position);
}

void player_reset(Player* player, Vec3* position) {
    fw64_character_controller_reset(&player->character_controller);

    quat_ident(&player->node->transform.rotation);

    if (position) {
        player->node->transform.position = *position;
        fw64_node_update(player->node);
    }
}

void player_update(Player* player) {
    if (player->process_input)
        process_input(player);

    fw64_character_controller_fixed_update(&player->character_controller, player->engine->time->time_delta);
}

void process_input(Player* player) {
    Vec2 stick;
    fw64_input_controller_stick(player->engine->input, 0, &stick);
    if (stick.x >= PLAYER_STICK_THRESHOLD || stick.x <= -PLAYER_STICK_THRESHOLD) {
        float rotation_delta = PLAYER_DEFAULT_ROTATION_SPEED * player->engine->time->time_delta;

        if (stick.x >= PLAYER_STICK_THRESHOLD) {
            player->character_controller.rotation -= rotation_delta;
        }
        else if (stick.x <= -PLAYER_STICK_THRESHOLD) {
            player->character_controller.rotation += rotation_delta;
        }
    }

    if (stick.y >= PLAYER_STICK_THRESHOLD ) {
        fw64_character_controller_accelerate(&player->character_controller);
    }
    else if (stick.y <= -PLAYER_STICK_THRESHOLD) {
        fw64_character_controller_decelerate(&player->character_controller);
    }

    if (fw64_input_controller_button_pressed(player->engine->input, player->controller_num, FW64_N64_CONTROLLER_BUTTON_A)) {
        if (player->character_controller.state == PLAYER_STATE_ON_GROUND) {
            fw64_character_controller_jump(&player->character_controller);
        }
    }
}

void player_calculate_size(Player* player) {
    Vec3 extents;
    box_extents(&player->node->collider->bounding, &extents);
    player->character_controller.height = extents.y * 2.0f;
    player->character_controller.radius = extents.x > extents.z ? extents.x : extents.z;
}

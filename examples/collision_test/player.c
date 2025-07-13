#include "player.h"

#include "framework64/controller_mapping/n64.h"

void player_init(Player* player, fw64Engine* engine, fw64CharacterEnvironment* env, fw64Scene* scene, fw64Node* node) {
    player->engine = engine;
    player->node = node;
    fw64_character_init(&player->character, env, scene);

    player->character.previous_position = player->node->transform.position;
    player->character.position = player->node->transform.position;
    box_size(&player->node->collider->bounding, &player->character.size);
}

void player_update(Player* player) {
    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        player->character.attempt_to_jump = 1;
    }

    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z)) {
        Vec3 pos = {0.0f, 7.0f, 0.0f};
        vw64_character_reset_position(&player->character, &pos);
    }

    // TODO: need to actually figure this out based on camera
    Vec2 stick;
    fw64_input_controller_stick(player->engine->input, 0, &stick);
    vec3_set(&player->character.attempt_to_move, stick.x, 0.0f, -stick.y);

    vec3_lerp(&player->character.previous_position, &player->character.position, player->engine->time->accumulator_progress, &player->node->transform.position);
    fw64_node_update(player->node);
}

void player_fixed_update(Player* player) {
    fw64_character_fixed_update(&player->character, player->engine->time->fixed_time_delta);
}

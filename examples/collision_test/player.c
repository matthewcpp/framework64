#include "player.h"

#include "framework64/controller_mapping/n64.h"

void player_init(Player* player, fw64Engine* engine, fw64CharacterEnvironment* env, fw64Scene* scene, fw64Node* node) {
    player->engine = engine;
    player->node = node;
    fw64_character_init(&player->character, env, scene);
}

static void player_do_jump(Player* player) {
    if (!fw64_character_is_on_ground(&player->character)) {
        return;
    }

    player->character.velocity.x = 5.0f;
}

void player_update(Player* player) {
    if (fw64_input_controller_button_pressed(player->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        player_do_jump(player);
    }

    fw64_character_update(&player->character, player->engine->time->time_delta);
}

#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/log.h"
#include "framework64/n64_libultra/data_link.h"

#include "framework64/data_link.h"

#include <stdio.h>
#include <string.h>

static void message_list_init(Game* game);
static void message_list_push(Game* game, fw64DataSource* message);
static void draw_data_link_messages(Game* game);

static void on_data_link_connected(void* arg) {
    Game* game = (Game*)arg;

    const char* start_message = "data link example started";
    fw64_data_link_send_message(game->engine->data_link, Fw64_DATA_LINK_MESSAGE_TEXT, start_message, strlen(start_message));
}

void on_data_link_message(fw64DataSource* message, void* arg) {
    Game* game = (Game*)arg;

    game->total_message_count += 1;
    game->total_message_data += fw64_data_source_size(message);
    message_list_push(game, message);
}

void game_init(Game* game, fw64Engine* engine) {
    memset(game, 0, sizeof(Game));
    game->engine = engine;

    fw64_camera_init(&game->camera, fw64_displays_get_primary(engine->displays));
    message_list_init(game);

    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas14, fw64_default_allocator());
    fw64_renderer_set_anti_aliasing_enabled(engine->renderer, 0);

    fw64_data_link_set_connected_callback(engine->data_link, on_data_link_connected, game);
    fw64_data_link_set_mesage_callback(engine->data_link, on_data_link_message, game);
}

void game_update(Game* game){
    if (fw64_input_controller_button_pressed(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_START)) {
        fw64_debug_log("Total Messages: %u Data: %u", game->total_message_count, game->total_message_data);
    }
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(game->engine->renderer, &game->camera);

    if (fw64_data_link_connected(game->engine->data_link)) {
        draw_data_link_messages(game);
    }
    else {
        fw64_renderer_draw_text(game->engine->renderer, game->font, 10, 10, "No data link connection.");
    }

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void draw_data_link_messages(Game* game) {
    char buffer[64];

    int y_pos = 10;

    sprintf(buffer, "Messages: %u Data: %u", game->total_message_count, game->total_message_data);
    fw64_renderer_draw_text(game->engine->renderer, game->font, 10, y_pos, buffer);

    for (size_t i = 0; i < MESSAGE_LIST_COUNT; i++) {
        y_pos += fw64_font_size(game->font) + 4;
        fw64_renderer_draw_text(game->engine->renderer, game->font, 10, y_pos, game->message_list[i]);
    }
}

void message_list_init(Game* game) {
    fw64Allocator* allocator = fw64_default_allocator();

    game->message_list = allocator->malloc(allocator, sizeof(char*) * MESSAGE_LIST_COUNT);

    for (size_t i = 0; i < MESSAGE_LIST_COUNT; i++) {
        char* message_buffer = allocator->malloc(allocator, MESSAGE_LIST_MESSAGE_SIZE);
        message_buffer[0] = 0;

        game->message_list[i] = message_buffer;
    }
}
void message_list_push(Game* game, fw64DataSource* message){
    char* message_buffer = game->message_list[MESSAGE_LIST_COUNT - 1];

    for (size_t i = MESSAGE_LIST_COUNT - 1; i > 0; i--) {
        game->message_list[i] = game->message_list[i - 1];
    }
    game->message_list[0] = message_buffer;

    size_t message_size = fw64_data_source_size(message);
    if (message_size < MESSAGE_LIST_MESSAGE_SIZE - 1) {
        fw64_data_source_read(message, &message_buffer[0], 1, message_size);
        message_buffer[message_size] = 0;
    }
    else {
        fw64_data_source_read(message, &message_buffer[0], 1, MESSAGE_LIST_MESSAGE_SIZE - 4);
        strcpy(&message_buffer[MESSAGE_LIST_MESSAGE_SIZE - 4], "...");
    }
}


#include "game.h"
#include "assets/assets.h"

#include "framework64/n64/controller_button.h"

#include <stdio.h>
#include <string.h>

#define ROTATION_SPEED 45.0f

static void draw_ui(Game* game);
static void read_data(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    game->engine = engine;
    memset(&game->save_data, 0, sizeof(SaveData));

    fw64_camera_init(&game->camera);

    fw64_node_init(&game->solid_cube);
    fw64_node_set_mesh(&game->solid_cube, fw64_assets_load_mesh(engine->assets, FW64_ASSET_mesh_blue_cube, allocator));
    
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);

    game->rotation = 0.0f;
    game->loaded = 0;
}

void game_update(Game* game){
    game->rotation += game->engine->time->time_delta * ROTATION_SPEED;

    quat_from_euler(&game->solid_cube.transform.rotation, 0, game->rotation, 0.0f);
    fw64_node_update(&game->solid_cube);

    if (!fw64_save_file_valid(game->engine->save_file))
        return;

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        read_data(game);
        game->loaded = 1;
    }

    if (game->loaded) {
        if (fw64_input_controller_button_pressed(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
            game->save_data.counter += 1;
        }
        else if (fw64_input_controller_button_pressed(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
            game->save_data.counter = game->save_data.counter > 0 ? game->save_data.counter - 1 : 0;
        }
        else if (fw64_input_controller_button_pressed(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_A)) {
            fw64_save_file_write(game->engine->save_file, 0, &game->save_data, sizeof(SaveData));
        }
    }
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_set_anti_aliasing_enabled(renderer, 1);
    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->camera);
    
    fw64_renderer_draw_static_mesh(renderer, &game->solid_cube.transform, game->solid_cube.mesh);

    fw64_renderer_set_anti_aliasing_enabled(renderer, 0);
    
    if (fw64_save_file_valid(game->engine->save_file)) {
        if (game->loaded)
            draw_ui(game);
        else
            fw64_renderer_draw_text(renderer, game->font, 20, 20, "Press Start to load data");
    }
    else {
        fw64_renderer_draw_text(renderer, game->font, 20, 20, "No Save File present");
    }
    
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

void draw_ui(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    char text[64];

    int draw_x = 20, draw_y = 20;
    sprintf(text, "Save size: %d", fw64_save_file_size(game->engine->save_file));
    fw64_renderer_draw_text(renderer, game->font, draw_x, draw_y, text);
    draw_y += 17;

    sprintf(text, "Counter: %llu", game->save_data.counter);
    fw64_renderer_draw_text(renderer, game->font, draw_x, draw_y, text);
    draw_y += 17;

    fw64_renderer_draw_text(renderer, game->font, draw_x, draw_y, "Use D-Pad to adjust counter");
    draw_y += 17;

    if (fw64_save_file_busy(game->engine->save_file))
        fw64_renderer_draw_text(renderer, game->font, draw_x, draw_y, "Saving...");
    else
        fw64_renderer_draw_text(renderer, game->font, draw_x, draw_y, "Press A to Save data");

    draw_y += 17;

    fw64_renderer_draw_text(renderer, game->font, draw_x, draw_y, "Press Start to Reload data");
    draw_y += 17;
}

#define SAVE_DATA_MAGIC 5566

void read_data(Game* game) {
    fw64_save_file_read(game->engine->save_file, 0, &game->save_data, sizeof(SaveData));
    
    if (game->save_data.magic != SAVE_DATA_MAGIC) { // first time loading
        memset(&game->save_data, 0, sizeof(SaveData));
        game->save_data.magic = SAVE_DATA_MAGIC;
        fw64_save_file_write(game->engine->save_file, 0, &game->save_data, sizeof(SaveData));
    } 
}

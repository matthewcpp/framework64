#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"
#include "framework64/util/text_util.h"

#include <stdio.h>
#include <string.h>

#define ROTATION_SPEED 45.0f

static void read_data(Game* game);
static void update_spritebatch(Game* game);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);
    
    game->engine = engine;
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    game->loaded = 0;
    game->spritebatch = fw64_spritebatch_create(1, allocator);
    memset(&game->save_data, 0, sizeof(SaveData));

    game->renderpasses[RENDER_PASS_SCENE] = fw64_renderpass_create(display, allocator);
    fw64Camera camera;
    fw64_camera_init(&camera, fw64_displays_get_primary(engine->displays));
    fw64_renderpass_set_camera(game->renderpasses[RENDER_PASS_SCENE], &camera);

    game->renderpasses[RENDER_PASS_UI] = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpasses[RENDER_PASS_UI]);

    fw64SceneInfo info;
    fw64_scene_info_init(&info);
    info.node_count = 1;
    info.mesh_count = 1;
    info.mesh_instance_count = 1;
    fw64_scene_init(&game->scene, &info, engine->assets, allocator);

    fw64Node* node = fw64_scene_create_node(&game->scene);
    fw64Mesh* mesh = fw64_scene_load_mesh_asset(&game->scene, FW64_ASSET_mesh_blue_cube);
    fw64_scene_create_mesh_instance(&game->scene, node, mesh);
    rotate_node_init(&game->rotate_node, node);
}

void game_update(Game* game){
    rotate_node_update(&game->rotate_node, game->engine->time->time_delta);

    if (!fw64_save_file_valid(game->engine->save_file))
        return;

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        read_data(game);
        game->loaded = 1;
    }

    if (game->loaded) {
        if (fw64_input_controller_button_pressed(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_DPAD_UP)) {
            game->save_data.counter += 1;
        }
        else if (fw64_input_controller_button_pressed(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN)) {
            game->save_data.counter = game->save_data.counter > 0 ? game->save_data.counter - 1 : 0;
        }
        else if (fw64_input_controller_button_pressed(game->engine->input, 0 , FW64_N64_CONTROLLER_BUTTON_A)) {
            fw64_save_file_write(game->engine->save_file, 0, &game->save_data, sizeof(SaveData));
        }
    }

    update_spritebatch(game);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES,  FW64_RENDERER_FLAG_CLEAR);
    fw64RenderPass* renderpass = game->renderpasses[RENDER_PASS_SCENE];
    fw64_renderpass_begin(renderpass);
    fw64_scene_draw_all(&game->scene, renderpass);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(renderer, renderpass);

    renderpass = game->renderpasses[RENDER_PASS_UI];
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_draw_sprite_batch(renderpass, game->spritebatch);
    fw64_renderpass_end(renderpass);
    fw64_renderer_submit_renderpass(renderer, renderpass);
    
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

void update_spritebatch(Game* game) {
    IVec2 display_size = fw64_display_get_size(fw64_displays_get_primary(game->engine->displays));
    char text[64];

    fw64_spritebatch_begin(game->spritebatch);

    if (!fw64_save_file_valid(game->engine->save_file)) {
        const char* message = "No Save File present";
        IVec2 pos = fw64_text_util_center_string(game->font, message, &display_size);
        fw64_spritebatch_draw_string(game->spritebatch, game->font, message, pos.x, pos.y);
        goto spritebatch_end;
    }

    if (!game->loaded) {
        const char* message = "Press Start to load data";
        IVec2 pos = fw64_text_util_center_string(game->font, message, &display_size);
        fw64_spritebatch_draw_string(game->spritebatch, game->font, message, pos.x, pos.y);
        goto spritebatch_end;
    }

    int draw_x = 20, draw_y = 20;
    sprintf(text, "Save size: %d", fw64_save_file_size(game->engine->save_file));
    fw64_spritebatch_draw_string(game->spritebatch, game->font, text, draw_x, draw_y);
    draw_y += 17;

    sprintf(text, "Counter: %llu", game->save_data.counter);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, text, draw_x, draw_y);
    draw_y += 17;

    fw64_spritebatch_draw_string(game->spritebatch, game->font, "Use D-Pad up/down to adjust counter", draw_x, draw_y);
    draw_y += 17;

    if (fw64_save_file_busy(game->engine->save_file))
        fw64_spritebatch_draw_string(game->spritebatch, game->font, "Saving...", draw_x, draw_y);
    else
        fw64_spritebatch_draw_string(game->spritebatch, game->font, "Press A to Save data", draw_x, draw_y);

    draw_y += 17;

    fw64_spritebatch_draw_string(game->spritebatch, game->font, "Press Start to Reload data", draw_x, draw_y);
    draw_y += 17;

spritebatch_end:
    fw64_spritebatch_end(game->spritebatch);
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

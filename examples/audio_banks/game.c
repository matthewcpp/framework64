#include "game.h"
#include "assets/assets.h"

#include "framework64/node.h"
#include "framework64/util/quad.h"
#include "framework64/util/texture_util.h"
#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_renderer_set_anti_aliasing_enabled(engine->renderer, 0);

    scene_view_init(&game->scene_view, engine);
    controller_init(&game->controller, engine, &game->audio_state);
    ui_init(&game->ui, engine, &game->audio_state);
}

void game_update(Game* game){
    scene_view_update(&game->scene_view);
    controller_update(&game->controller);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    scene_view_draw(&game->scene_view);
    ui_draw(&game->ui);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

// --- Scene View

void scene_view_init(SceneView* scene_view, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    scene_view->engine = engine;

    fw64SceneInfo scene_info;
    fw64_scene_info_init(&scene_info);
    scene_info.node_count = 1;
    fw64_scene_init(&scene_view->scene, &scene_info, engine->assets, allocator);

    fw64Node* node = fw64_scene_get_node(&scene_view->scene, 0);
    fw64_node_set_mesh(node, fw64_textured_quad_create(engine, FW64_ASSET_image_n64_logo, allocator));

    rotate_node_init(&scene_view->rotate_node, node);
    vec3_set(&scene_view->rotate_node.axis, 0.0f, 0.0f, 1.0f);

    scene_view->renderpass = fw64_renderpass_create(display, allocator);
    fw64Camera camera;
    fw64_camera_init(&camera, display);
    fw64_renderpass_set_camera(scene_view->renderpass, &camera);

    // scene is static so only need to setup renderpass drawing once
    fw64_renderpass_begin(scene_view->renderpass);
    fw64_renderpass_draw_static_mesh(scene_view->renderpass, node->mesh, &node->transform);
    fw64_renderpass_end(scene_view->renderpass);
}

void scene_view_update(SceneView* scene_view) {
    rotate_node_update(&scene_view->rotate_node, scene_view->engine->time->time_delta);
}

void scene_view_draw(SceneView* scene_view) {
    fw64_renderer_submit_renderpass(scene_view->engine->renderer, scene_view->renderpass);
}

// --- Controller
#define SOUND_BANK_COUNT 2
#define MUSIC_BANK_COUNT 2
static int sound_banks[SOUND_BANK_COUNT] = { FW64_ASSET_soundbank_soundbank1, FW64_ASSET_soundbank_soundbank2 };
static int music_banks[MUSIC_BANK_COUNT] = { FW64_ASSET_musicbank_musicbank1, FW64_ASSET_musicbank_musicbank2 };

static void controller_change_sound_bank(Game* game, int delta) {
    game->sound_bank += delta;

    if (game->sound_bank < 0) {
        game->sound_bank = 0;
    }

    if (game->sound_bank >= SOUND_BANK_COUNT) {
        game->sound_bank = SOUND_BANK_COUNT - 1;
    }

    fw64_audio_load_soundbank_asset(game->engine->audio, game->engine->assets, sound_banks[game->sound_bank]);

    game->sound_num = 0;
}

static void controller_change_sound(Game* game, int delta) {
    int sound_count = (int)fw64_audio_sound_count(game->engine->audio);
    game->sound_num += delta;

    if (game->sound_num < 0) {
        game->sound_num = 0;
    }

    if (game->sound_num >= sound_count) {
        game->sound_num = sound_count - 1;
    }
}

static void controller_change_music_bank(Game* game, int delta) {
    game->music_bank += delta;

    if (game->music_bank < 0) {
        game->music_bank = 0;
    }

    if (game->music_bank >= MUSIC_BANK_COUNT) {
        game->music_bank = MUSIC_BANK_COUNT - 1;
    }

    fw64_audio_load_musicbank_asset(game->engine->audio, game->engine->assets,  music_banks[game->music_bank]);

    game->music_track = 0;
}

static void controller_change_music_track(Game* game, int delta) {
    int music_track_count = (int)fw64_audio_music_track_count(game->engine->audio);
    game->music_track += delta;

    if (game->music_track < 0) {
        game->music_track = 0;
    }

    if (game->music_track >= music_track_count) {
        game->music_track = music_track_count - 1;
    }
}

void controller_init(Controller* controller, fw64Engine* engine, AudioInfo* info){
    controller->engine = engine;

    controller->sound_bank = -1;
    controller_change_sound_bank(game, 1);

    controller->music_bank = -1;
    controller_change_music_bank(game, 1);
}

void controller_update(Controller* controller) {
    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_A))
        controller->sound_id = fw64_audio_play_sound(controller->engine->audio, controller->sound_num);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_B))
        fw64_audio_stop_sound(controller->engine->audio, controller->sound_id);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT))
        change_sound(controller, 1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT))
        change_sound(controller, -1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP))
        change_sound_bank(controller, 1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN))
        change_sound_bank(controller, -1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START))
        fw64_audio_play_music(controller->engine->audio, controller->music_track);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z))
        fw64_audio_stop_music(controller->engine->audio);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT))
        change_music_track(controller, 1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT))
        change_music_track(controller, -1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP))
        change_music_bank(controller, 1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN))
        change_music_bank(controller, -1);
}

// --- UI

static void draw_sound_controls(UI* ui) {
    fw64Renderer* renderer = ui->engine->renderer;
    char buffer[32];

    fw64_renderer_draw_text(renderer, ui->font, 10, 10, "Sounds");
    fw64_renderer_draw_text(renderer, ui->font, 10, 25, "Bank:");
    sprintf(buffer, "%d", ui->sound_bank);
    fw64_renderer_draw_text(renderer, ui->font, 70, 25, buffer);

    fw64_renderer_draw_text(renderer, ui->font, 10, 40, "Sound:");
    sprintf(buffer, "%d", ui->sound_num);
    fw64_renderer_draw_text(renderer, ui->font, 70, 40, buffer);

    fw64_renderer_draw_text(renderer, ui->font, 30, 55, "Play");
    fw64_renderer_draw_text(renderer, ui->font, 30, 70, "Stop");

    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 3, 53, 25);
    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 2, 80, 25);
    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 4, 53, 40);
    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 5, 80, 40);

    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 0, 10, 55);
    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 1, 10, 70);
}

static void draw_music_controls(UI* ui) {
    fw64Renderer* renderer = ui->engine->renderer;
    char buffer[32];

    int offset = 200;

    fw64_renderer_draw_text(renderer, ui->font, offset + 10, 10, "Music");
    fw64_renderer_draw_text(renderer, ui->font, offset + 10, 25, "Bank:");
    sprintf(buffer, "%d", ui->music_bank);
    fw64_renderer_draw_text(renderer, ui->font, offset + 70, 25, buffer);

    fw64_renderer_draw_text(renderer, ui->font, offset + 10, 40, "Track:");
    sprintf(buffer, "%d", ui->music_track);
    fw64_renderer_draw_text(renderer, ui->font, offset + 70, 40, buffer);

    fw64_renderer_draw_text(renderer, ui->font, offset + 30, 55, "Play");
    fw64_renderer_draw_text(renderer, ui->font, offset + 30, 70, "Stop");

    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 8, offset + 53, 25);
    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 9, offset + 80, 25);
    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 10, offset + 53, 40);
    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 11, offset + 80, 40);

    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 12, offset + 10, 55);
    fw64_renderer_draw_sprite_slice(renderer, ui->buttons, 13, offset + 10, 70);
}

void void ui_init(UI* ui, fw64Engine* engine, AudioInfo* info) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    ui->engine = engine;
    ui->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui->buttons = fw64_texture_util_create_from_loaded_image(engine->assets, FW64_ASSET_image_buttons, allocator);
    ui->renderpass = fw64_renderpass_create(display, allocator);
    ui->sprite_batch = fw64_spritebatch_create(1, allocator);
}

void ui_draw(UI* ui) {

}
#include "game.h"
#include "assets/assets.h"

#include "framework64/node.h"
#include "framework64/util/quad.h"
#include "framework64/util/renderpass_util.h"
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
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
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
    scene_info.node_count = 2;
    scene_info.mesh_instance_count = 1;
    fw64_scene_init(&scene_view->scene, &scene_info, engine->assets, allocator);

    fw64Node* node = fw64_scene_create_node(&scene_view->scene);
    fw64Mesh* mesh = fw64_textured_quad_create(engine, FW64_ASSET_image_n64_logo, allocator);
    fw64_scene_create_mesh_instance(&scene_view->scene, node, mesh);

    fw64_rotate_node_init(&scene_view->rotate_node, node);
    vec3_set(&scene_view->rotate_node.axis, 0.0f, 0.0f, 1.0f);

    scene_view->renderpass = fw64_renderpass_create(display, allocator);

    fw64Node* camera_node = fw64_scene_create_node(&scene_view->scene);
    vec3_set(&camera_node->transform.position, 0.0f, 0.0f, 5.0f);
    fw64_node_update(camera_node);
    fw64Camera camera;
    fw64_camera_init(&camera, camera_node, display);
    fw64_renderpass_set_camera(scene_view->renderpass, &camera);

    // scene is static so only need to setup renderpass drawing once
    fw64_renderpass_begin(scene_view->renderpass);
    fw64_scene_draw_all(&scene_view->scene, scene_view->renderpass);
    fw64_renderpass_end(scene_view->renderpass);
}

void scene_view_update(SceneView* scene_view) {
    fw64_rotate_node_update(&scene_view->rotate_node, scene_view->engine->time->time_delta);
}

void scene_view_draw(SceneView* scene_view) {
    fw64_renderer_submit_renderpass(scene_view->engine->renderer, scene_view->renderpass);
}

// --- Controller
#define SOUND_BANK_COUNT 2
#define MUSIC_BANK_COUNT 2
static int sound_banks[SOUND_BANK_COUNT] = { FW64_ASSET_soundbank_soundbank1, FW64_ASSET_soundbank_soundbank2 };
static int music_banks[MUSIC_BANK_COUNT] = { FW64_ASSET_musicbank_musicbank1, FW64_ASSET_musicbank_musicbank2 };

static void controller_change_sound_bank(Controller* controller, int delta) {
    controller->audio_state->sound_bank += delta;

    if (controller->audio_state->sound_bank < 0) {
        controller->audio_state->sound_bank = 0;
    }

    if (controller->audio_state->sound_bank >= SOUND_BANK_COUNT) {
        controller->audio_state->sound_bank = SOUND_BANK_COUNT - 1;
    }

    fw64_audio_load_soundbank_asset(controller->engine->audio, controller->engine->assets, sound_banks[controller->audio_state->sound_bank]);

    controller->audio_state->sound_num = 0;
}

static void controller_change_sound(Controller* controller, int delta) {
    int sound_count = (int)fw64_audio_sound_count(controller->engine->audio);
    controller->audio_state->sound_num += delta;

    if (controller->audio_state->sound_num < 0) {
        controller->audio_state->sound_num = 0;
    }

    if (controller->audio_state->sound_num >= sound_count) {
        controller->audio_state->sound_num = sound_count - 1;
    }
}

static void controller_change_music_bank(Controller* controller, int delta) {
    controller->audio_state->music_bank += delta;

    if (controller->audio_state->music_bank < 0) {
        controller->audio_state->music_bank = 0;
    }

    if (controller->audio_state->music_bank >= MUSIC_BANK_COUNT) {
        controller->audio_state->music_bank = MUSIC_BANK_COUNT - 1;
    }

    fw64_audio_load_musicbank_asset(controller->engine->audio, controller->engine->assets,  music_banks[controller->audio_state->music_bank]);

    controller->audio_state->music_track = 0;
}

static void controller_change_music_track(Controller* controller, int delta) {
    int music_track_count = (int)fw64_audio_music_track_count(controller->engine->audio);
    controller->audio_state->music_track += delta;

    if (controller->audio_state->music_track < 0) {
        controller->audio_state->music_track = 0;
    }

    if (controller->audio_state->music_track >= music_track_count) {
        controller->audio_state->music_track = music_track_count - 1;
    }
}

void controller_init(Controller* controller, fw64Engine* engine, AudioState* audio_state){
    controller->engine = engine;
    controller->audio_state = audio_state;

    controller->audio_state->sound_bank = -1;
    controller_change_sound_bank(controller, 1);

    controller->audio_state->music_bank = -1;
    controller_change_music_bank(controller, 1);
}

void controller_update(Controller* controller) {
    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_A))
        controller->audio_state->sound_id = fw64_audio_play_sound(controller->engine->audio, controller->audio_state->sound_num);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_B))
        fw64_audio_stop_sound(controller->engine->audio, controller->audio_state->sound_id);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT))
        controller_change_sound(controller, 1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT))
        controller_change_sound(controller, -1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP))
        controller_change_sound_bank(controller, 1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN))
        controller_change_sound_bank(controller, -1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START))
        fw64_audio_play_music(controller->engine->audio, controller->audio_state->music_track);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z))
        fw64_audio_stop_music(controller->engine->audio);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT))
        controller_change_music_track(controller, 1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT))
        controller_change_music_track(controller, -1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP))
        controller_change_music_bank(controller, 1);

    if (fw64_input_controller_button_pressed(controller->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN))
        controller_change_music_bank(controller, -1);
}

// --- UI

static void draw_sound_controls(UI* ui) {
    fw64SpriteBatch* sprite_batch = ui->sprite_batch;
    char buffer[32];

    fw64_spritebatch_draw_string(sprite_batch, ui->font, "Sounds", 10, 10);
    fw64_spritebatch_draw_string(sprite_batch, ui->font, "Bank:", 10, 25);
    sprintf(buffer, "%d", ui->audio_state->sound_bank);
    fw64_spritebatch_draw_string(sprite_batch, ui->font, buffer, 70, 25);

    fw64_spritebatch_draw_string(sprite_batch, ui->font, "Sound:", 10, 40);
    sprintf(buffer, "%d", ui->audio_state->sound_num);
    fw64_spritebatch_draw_string(sprite_batch, ui->font, buffer, 70, 40);

    fw64_spritebatch_draw_string(sprite_batch, ui->font, "Play", 30, 55);
    fw64_spritebatch_draw_string(sprite_batch, ui->font, "Stop", 30, 70);

    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 3, 53, 25);
    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 2, 80, 25);
    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 4, 53, 40);
    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 5, 80, 40);

    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 0, 10, 55);
    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 1, 10, 70);
}

static void draw_music_controls(UI* ui) {
    fw64SpriteBatch* sprite_batch = ui->sprite_batch;
    char buffer[32];

    int offset = 200;

    fw64_spritebatch_draw_string(sprite_batch, ui->font, "Music", offset + 10, 10);
    fw64_spritebatch_draw_string(sprite_batch, ui->font, "Bank:", offset + 10, 25);
    sprintf(buffer, "%d", ui->audio_state->music_bank);
    fw64_spritebatch_draw_string(sprite_batch, ui->font, buffer, offset + 70, 25);

    fw64_spritebatch_draw_string(sprite_batch, ui->font, "Track:", offset + 10, 40);
    sprintf(buffer, "%d", ui->audio_state->music_track);
    fw64_spritebatch_draw_string(sprite_batch, ui->font, buffer, offset + 70, 40);

    fw64_spritebatch_draw_string(sprite_batch, ui->font, "Play", offset + 30, 55);
    fw64_spritebatch_draw_string(sprite_batch, ui->font, "Stop", offset + 30, 70);

    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 8, offset + 53, 25);
    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 9, offset + 80, 25);
    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 10, offset + 53, 40);
    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 11, offset + 80, 40);

    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 12, offset + 10, 55);
    fw64_spritebatch_draw_sprite_slice(sprite_batch, ui->buttons, 13, offset + 10, 70);
}

void ui_init(UI* ui, fw64Engine* engine, AudioState* audio_state) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    ui->engine = engine;
    ui->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    ui->buttons = fw64_texture_util_create_from_loaded_image(engine->assets, FW64_ASSET_image_buttons, allocator);
    ui->sprite_batch = fw64_spritebatch_create(1, allocator);
    ui->audio_state = audio_state;

    ui->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(ui->renderpass);
}

void ui_draw(UI* ui) {
    fw64_spritebatch_begin(ui->sprite_batch);
    draw_sound_controls(ui);
    draw_music_controls(ui);
    fw64_spritebatch_end(ui->sprite_batch);

    fw64_renderpass_begin(ui->renderpass);
    fw64_renderpass_draw_sprite_batch(ui->renderpass, ui->sprite_batch);
    fw64_renderpass_end(ui->renderpass);

    fw64_renderer_submit_renderpass(ui->engine->renderer, ui->renderpass);
}

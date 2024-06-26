#include "game.h"
#include "assets/assets.h"

#include "framework64/node.h"
#include "framework64/util/quad.h"
#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

#define SOUND_BANK_COUNT 2
#define MUSIC_BANK_COUNT 2
static int sound_banks[SOUND_BANK_COUNT] = { FW64_ASSET_soundbank_soundbank1, FW64_ASSET_soundbank_soundbank2 };
static int music_banks[MUSIC_BANK_COUNT] = { FW64_ASSET_musicbank_musicbank1, FW64_ASSET_musicbank_musicbank2 };

void change_sound_bank(Game* game, int delta);
void change_sound(Game* game, int delta);

void change_music_bank(Game* game, int delta);
void change_music_track(Game* game, int delta);

#define ROTATION_SPEED -M_PI / 4.0f

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera, fw64_displays_get_primary(engine->displays));

    fw64_renderer_set_anti_aliasing_enabled(engine->renderer, 0);

    game->sound_bank = -1;
    change_sound_bank(game, 1);

    game->music_bank = -1;
    change_music_bank(game, 1);

    fw64Allocator* allocator = fw64_default_allocator();

    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
    game->buttons = fw64_texture_create_from_image(fw64_assets_load_image(engine->assets, FW64_ASSET_image_buttons, allocator), allocator);

    game->sound_id = 0;

    fw64_node_init(&game->n64_logo);
    fw64_node_set_mesh(&game->n64_logo, fw64_textured_quad_create(game->engine, FW64_ASSET_image_n64_logo, allocator));

    game->rotation = 0.0f;
}

void game_update(Game* game){
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_A))
        game->sound_id = fw64_audio_play_sound(game->engine->audio, game->sound_num);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_B))
        fw64_audio_stop_sound(game->engine->audio, game->sound_id);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT))
        change_sound(game, 1);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT))
        change_sound(game, -1);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_UP))
        change_sound_bank(game, 1);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_DOWN))
        change_sound_bank(game, -1);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START))
        fw64_audio_play_music(game->engine->audio, game->music_track);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_Z))
        fw64_audio_stop_music(game->engine->audio);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT))
        change_music_track(game, 1);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT))
        change_music_track(game, -1);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_UP))
        change_music_bank(game, 1);

    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_DOWN))
        change_music_bank(game, -1);

    game->rotation += ROTATION_SPEED * game->engine->time->time_delta;
    quat_set_axis_angle(&game->n64_logo.transform.rotation, 0.0f, 0.0f, 1.0f, game->rotation);
    fw64_node_update(&game->n64_logo);
}

static void draw_sound_controls(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    char buffer[32];

    fw64_renderer_draw_text(renderer, game->font, 10, 10, "Sounds");
    fw64_renderer_draw_text(renderer, game->font, 10, 25, "Bank:");
    sprintf(buffer, "%d", game->sound_bank);
    fw64_renderer_draw_text(renderer, game->font, 70, 25, buffer);

    fw64_renderer_draw_text(renderer, game->font, 10, 40, "Sound:");
    sprintf(buffer, "%d", game->sound_num);
    fw64_renderer_draw_text(renderer, game->font, 70, 40, buffer);

    fw64_renderer_draw_text(renderer, game->font, 30, 55, "Play");
    fw64_renderer_draw_text(renderer, game->font, 30, 70, "Stop");

    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 3, 53, 25);
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 2, 80, 25);
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 4, 53, 40);
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 5, 80, 40);

    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 0, 10, 55);
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 1, 10, 70);
}

static void draw_music_controls(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;
    char buffer[32];

    int offset = 200;

    fw64_renderer_draw_text(renderer, game->font, offset + 10, 10, "Music");
    fw64_renderer_draw_text(renderer, game->font, offset + 10, 25, "Bank:");
    sprintf(buffer, "%d", game->music_bank);
    fw64_renderer_draw_text(renderer, game->font, offset + 70, 25, buffer);

    fw64_renderer_draw_text(renderer, game->font, offset + 10, 40, "Track:");
    sprintf(buffer, "%d", game->music_track);
    fw64_renderer_draw_text(renderer, game->font, offset + 70, 40, buffer);

    fw64_renderer_draw_text(renderer, game->font, offset + 30, 55, "Play");
    fw64_renderer_draw_text(renderer, game->font, offset + 30, 70, "Stop");

    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 8, offset + 53, 25);
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 9, offset + 80, 25);
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 10, offset + 53, 40);
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 11, offset + 80, 40);

    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 12, offset + 10, 55);
    fw64_renderer_draw_sprite_slice(renderer, game->buttons, 13, offset + 10, 70);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->camera);
    fw64_renderer_draw_static_mesh(renderer, &game->n64_logo.transform, game->n64_logo.mesh);

    draw_sound_controls(game);
    draw_music_controls(game);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

void change_sound_bank(Game* game, int delta) {
    game->sound_bank += delta;

    if (game->sound_bank < 0)
        game->sound_bank = 0;

    if (game->sound_bank >= SOUND_BANK_COUNT)
        game->sound_bank = SOUND_BANK_COUNT - 1;

    fw64_audio_load_soundbank_asset(game->engine->audio, game->engine->assets, sound_banks[game->sound_bank]);

    game->sound_num = 0;
}

void change_sound(Game* game, int delta) {
    int sound_count = (int)fw64_audio_sound_count(game->engine->audio);
    game->sound_num += delta;

    if (game->sound_num < 0)
        game->sound_num = 0;

    if (game->sound_num >= sound_count)
        game->sound_num = sound_count - 1;
}

void change_music_bank(Game* game, int delta) {
    game->music_bank += delta;

    if (game->music_bank < 0)
        game->music_bank = 0;

    if (game->music_bank >= MUSIC_BANK_COUNT)
        game->music_bank = MUSIC_BANK_COUNT - 1;

    fw64_audio_load_musicbank_asset(game->engine->audio, game->engine->assets,  music_banks[game->music_bank]);

    game->music_track = 0;
}

void change_music_track(Game* game, int delta) {
    int music_track_count = (int)fw64_audio_music_track_count(game->engine->audio);
    game->music_track += delta;

    if (game->music_track < 0)
        game->music_track = 0;

    if (game->music_track >= music_track_count)
        game->music_track = music_track_count - 1;
}

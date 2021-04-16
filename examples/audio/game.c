#include "game.h"
#include "assets.h"

#include <nusys.h>

#include "framework64/util/quad.h"

#include <malloc.h>
#include <math.h>

#include "framework64/filesystem.h"

#define SOUND_BANK_COUNT 2
#define MUSIC_BANK_COUNT 2
static int sound_banks[SOUND_BANK_COUNT] = { ASSET_soundbank_bank1, ASSET_soundbank_bank2 };
static int music_banks[MUSIC_BANK_COUNT] = { ASSET_musicbank_bank1, ASSET_musicbank_bank2 };

void change_sound_bank(Game* game, int delta);
void change_sound(Game* game, int delta);

void change_music_bank(Game* game, int delta);
void change_music_track(Game* game, int delta);

#define ROTATION_SPEED -M_PI / 4.0f

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    camera_init(&game->camera);

    game->sound_bank = -1;
    change_sound_bank(game, 1);

    game->music_bank = -1;
    change_music_bank(game, 1);

    game->font = fw64_assets_get_font(engine->assets, ASSET_font_Consolas12);
    game->buttons = fw64_assets_get_image(engine->assets, ASSET_sprite_buttons);

    game->sound_id = 0;

    Mesh* mesh = malloc(sizeof(Mesh));
    textured_quad_create(mesh, fw64_assets_get_image(engine->assets, ASSET_sprite_n64_logo));
    entity_init(&game->n64_logo, mesh);

    game->rotation = 0.0f;
}

void game_update(Game* game, float time_delta){
    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_A))
        game->sound_id = fw64_audio_play_sound(game->engine->audio, game->sound_num);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_B))
        fw64_audio_stop_sound(game->engine->audio, game->sound_id);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_C_RIGHT))
        change_sound(game, 1);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_C_LEFT))
        change_sound(game, -1);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_C_UP))
        change_sound_bank(game, 1);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_C_DOWN))
        change_sound_bank(game, -1);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_START))
        fw64_audio_play_music(game->engine->audio, game->music_track);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_Z))
        fw64_audio_stop_music(game->engine->audio);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_DPAD_RIGHT))
        change_music_track(game, 1);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_DPAD_LEFT))
        change_music_track(game, -1);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_DPAD_UP))
        change_music_bank(game, 1);

    if (input_button_pressed(game->engine->input, 0, CONTROLLER_BUTTON_DPAD_DOWN))
        change_music_bank(game, -1);

    game->rotation += ROTATION_SPEED * time_delta;
    quat_set_axis_angle(&game->n64_logo.transform.rotation, 0.0f, 0.0f, 1.0f, game->rotation);
    entity_refresh(&game->n64_logo);
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

    fw64_renderer_begin(renderer, &game->camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);

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
    
    fw64_audio_load_soundbank(game->engine->audio, sound_banks[game->sound_bank]);

    game->sound_num = 0;
}

void change_sound(Game* game, int delta) {
    game->sound_num += delta;

    if (game->sound_num < 0) 
        game->sound_num = 0;
    
    if (game->sound_num >= game->engine->audio->sound_count)
        game->sound_num = game->engine->audio->sound_count - 1;
}

void change_music_bank(Game* game, int delta) {
    game->music_bank += delta;

    if (game->music_bank < 0)
        game->music_bank = 0;

    if (game->music_bank >= MUSIC_BANK_COUNT) 
        game->music_bank = MUSIC_BANK_COUNT - 1;

    fw64_audio_load_music(game->engine->audio, music_banks[game->music_bank]);

    game->music_track = 0;
}

void change_music_track(Game* game, int delta) {
    game->music_track += delta;

    if (game->music_track < 0)
        game->music_track = 0;

    if (game->music_track >= game->engine->audio->music_track_count)
        game->music_track = game->engine->audio->music_track_count - 1;
}

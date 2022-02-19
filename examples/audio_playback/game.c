#include "game.h"
#include "assets.h"

#include "framework64/node.h"
#include "framework64/util/quad.h"
#include "framework64/n64/controller_button.h"

#include <stdio.h>

#define ROTATION_SPEED -M_PI / 4.0f

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);

    fw64_renderer_set_anti_aliasing_enabled(engine->renderer, 0);

    game->font = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12, NULL);
    game->buttons = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_buttons, NULL), NULL);
    game->music_bank = fw64_music_bank_load(engine->assets, FW64_ASSET_musicbank_musicbank2, NULL);
    fw64_node_init(&game->n64_logo);
    fw64_node_set_mesh(&game->n64_logo, fw64_textured_quad_create(game->engine, FW64_ASSET_image_n64_logo, NULL));

    fw64_audio_set_music_bank(engine->audio, game->music_bank);
    fw64_audio_play_music(engine->audio, 2);
    game->music_playback_speed = 1.0f;

    game->rotation = 0.0f;
}

void game_update(Game* game){
    fw64AudioStatus music_status = fw64_audio_get_music_status(game->engine->audio);

    if (music_status == FW64_AUDIO_PLAYING) {
        game->rotation += (ROTATION_SPEED * game->music_playback_speed) * game->engine->time->time_delta;
        quat_set_axis_angle(&game->n64_logo.transform.rotation, 0.0f, 0.0f, 1.0f, game->rotation);
        fw64_node_update(&game->n64_logo);
    }

    if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        game->music_playback_speed += 0.1f;
        fw64_audio_set_music_playback_speed(game->engine->audio, game->music_playback_speed);
    }
    else if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        game->music_playback_speed -= 0.1f;
        fw64_audio_set_music_playback_speed(game->engine->audio, game->music_playback_speed);
    }

    if (music_status == FW64_AUDIO_STOPPED) {
            fw64_audio_play_music(game->engine->audio, 2);
    }

    sprintf(game->tempo_text, "Playback Speed: %.1f", fw64_audio_get_playback_speed(game->engine->audio));
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_set_camera(renderer, &game->camera);
    fw64_renderer_draw_static_mesh(renderer, &game->n64_logo.transform, game->n64_logo.mesh);
    fw64_renderer_draw_text(renderer, game->font, 30, 200, game->tempo_text);

    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

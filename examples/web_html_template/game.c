#include "game.h"
#include "assets/assets.h"
#include "assets/music_bank_music.h"
#include <framework64/util/renderpass_util.h>

#include "framework64/controller_mapping/n64.h"

#include <stdio.h>

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    game->engine = engine;

    fw64_audio_load_musicbank_asset(engine->audio, engine->assets, FW64_ASSET_musicbank_music);
    // music should play from the start due to the game not starting until the user clicks the html button and allows audio to be played.
    fw64_audio_play_music(engine->audio, music_bank_music_gfaydark);

    game->renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), allocator);
    fw64_renderpass_util_ortho2d(game->renderpass);
    game->spritebatch = fw64_spritebatch_create(1, allocator);
    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Consolas12, allocator);
}

void game_update(Game* game){
    fw64_spritebatch_begin(game->spritebatch);

    // verify that the controller API is working
    // note: we do not enable keyboard controller emulation for this example
    char buffer[32];
    int x_pos = 20;
    int y_pos = 20;
    for (int i = 0; i < 4; i++) {
        if (fw64_input_controller_is_connected(game->engine->input, i)) {
            sprintf(buffer, "Controller %d connected", i);
        } else {
            sprintf(buffer, "Controller %d disconnected", i);
        }

        fw64_spritebatch_draw_string(game->spritebatch, game->font, buffer, x_pos, y_pos);
        y_pos += fw64_font_line_height(game->font);
    }

    fw64_spritebatch_end(game->spritebatch);
}

void game_fixed_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_draw_sprite_batch(game->renderpass, game->spritebatch);
    fw64_renderpass_end(game->renderpass);

    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
}

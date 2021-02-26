#include "game.h"
#include "assets.h"

#include <nusys.h>

void change_bank(Game* game, int delta);
void change_sound(Game* game, int delta);

void game_init(Game* game, System* system) {
    game->system = system;
    camera_init(&game->camera);

    game->sound_bank = -1;
    change_bank(game, 1);

    game->font = assets_get_font(system->assets, ASSET_font_Consolas12);
    game->buttons = assets_get_image(system->assets, ASSET_sprite_buttons);

    game->sound_id = 0;
}

void game_update(Game* game, float time_delta){
    if (input_button_pressed(game->system->input, 0, A_BUTTON)) {
        game->sound_id = audio_play_sound(game->system->audio, game->sound_num);
    }

    if (input_button_pressed(game->system->input, 0, B_BUTTON)) {
        audio_stop_sound(game->system->audio, game->sound_id);
    }

    if (input_button_pressed(game->system->input, 0, R_CBUTTONS)) {
        change_sound(game, 1);
    }

    if (input_button_pressed(game->system->input, 0, L_CBUTTONS)) {
        change_sound(game, -1);
    }

    if (input_button_pressed(game->system->input, 0, U_CBUTTONS)) {
        change_bank(game, 1);
    }

    if (input_button_pressed(game->system->input, 0, D_CBUTTONS)) {
        change_bank(game, -1);
    }
}


void game_draw(Game* game) {
    Renderer* renderer = game->system->renderer;
    char buffer[32];

    renderer_begin(renderer, &game->camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);

    renderer_draw_text(renderer, game->font, 10, 10, "Sounds");
    renderer_draw_text(renderer, game->font, 10, 25, "Bank:");
    sprintf(buffer, "%d", game->sound_bank);
    renderer_draw_text(renderer, game->font, 70, 25, buffer);

    renderer_draw_text(renderer, game->font, 10, 40, "Sound:");
    sprintf(buffer, "%d", game->sound_num);
    renderer_draw_text(renderer, game->font, 70, 40, buffer);

    renderer_draw_text(renderer, game->font, 30, 55, "Play");
    renderer_draw_text(renderer, game->font, 30, 70, "Stop");

    renderer_draw_sprite_slice(renderer, game->buttons, 3, 53, 25);
    renderer_draw_sprite_slice(renderer, game->buttons, 2, 80, 25);
    renderer_draw_sprite_slice(renderer, game->buttons, 4, 53, 40);
    renderer_draw_sprite_slice(renderer, game->buttons, 5, 80, 40);

    renderer_draw_sprite_slice(renderer, game->buttons, 0, 10, 55);
    renderer_draw_sprite_slice(renderer, game->buttons, 1, 10, 70);

    renderer_end(game->system->renderer, RENDERER_FLAG_SWAP);
}

void change_bank(Game* game, int delta) {
    game->sound_bank += delta;

    if (game->sound_bank < 0)
        game->sound_bank = 0;

    if (game->sound_bank >= 2)
        game->sound_bank = 1;
    
    audio_load_soundbank(game->system->audio, ASSET_soundbank_bank1 + game->sound_bank );

    game->sound_num = 0;
}

void change_sound(Game* game, int delta) {
    game->sound_num += delta;

    if (game->sound_num < 0) 
        game->sound_num = 0;
    
    if (game->sound_num >= game->system->audio->sound_count)
        game->sound_num = game->system->audio->sound_count - 1;
}

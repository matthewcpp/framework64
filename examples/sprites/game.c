#include "game.h"
#include "assets.h"

#include <stdio.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_renderer_set_clear_color(game->engine->renderer, 0, 0, 255);

    fw64_camera_init(&game->camera);

    n64_logo_sprite_init(&game->n64logo, fw64_image_load(game->engine->assets, FW64_ASSET_image_n64_logo, NULL));
    game->n64logo.position.x = 10;
    game->n64logo.position.y = 10;

    ken_sprite_init(&game->ken_sprite, fw64_image_load(game->engine->assets, FW64_ASSET_image_ken, NULL));

    IVec2 screen_size;
    fw64_renderer_get_screen_size(game->engine->renderer, &screen_size);
    game->ken_sprite.position.x = 10;
    game->ken_sprite.position.y = screen_size.y - 10 - fw64_texture_height(game->ken_sprite.sprite);

    elapsed_time_init(&game->elapsed_time, fw64_font_load(engine->assets, FW64_ASSET_font_basicLAZER, NULL));
}

void game_update(Game* game){
    ken_sprite_update(&game->ken_sprite, game->engine->time->time_delta);
    elapsed_time_update(&game->elapsed_time, game->engine->time->time_delta);
}

void game_draw(Game* game){
    fw64_renderer_begin(game->engine->renderer, &game->camera, FW64_RENDERER_MODE_ORTHO2D, FW64_RENDERER_FLAG_CLEAR);
    n64_logo_sprite_draw(&game->n64logo, game->engine->renderer);
    ken_sprite_draw(&game->ken_sprite, game->engine->renderer);
    elapsed_time_draw(&game->elapsed_time, game->engine->renderer);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
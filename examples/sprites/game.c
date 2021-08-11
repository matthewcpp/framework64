#include "game.h"
#include "assets.h"

#include <stdio.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64_camera_init(&game->camera);

    n64_logo_sprite_init(&game->n64logo, fw64_assets_get_image(game->engine->assets, ASSET_sprite_n64_logo));
    game->n64logo.position.x = 10;
    game->n64logo.position.y = 10;

    ken_sprite_init(&game->ken_sprite, fw64_assets_get_image(game->engine->assets, ASSET_sprite_ken));

    IVec2 screen_size;
    fw64_renderer_get_screen_size(game->engine->renderer, &screen_size);
    game->ken_sprite.position.x = 10;
    game->ken_sprite.position.y = screen_size.y - 10 - game->ken_sprite.sprite->height;

    elapsed_time_init(&game->elapsed_time, fw64_assets_get_font(engine->assets, ASSET_font_basicLAZER));
}

void game_update(Game* game, float time_delta){
    ken_sprite_update(&game->ken_sprite, time_delta);
    elapsed_time_update(&game->elapsed_time, time_delta);
}

void game_draw(Game* game){
    fw64_renderer_begin(game->engine->renderer, &game->camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    n64_logo_sprite_draw(&game->n64logo, game->engine->renderer);
    ken_sprite_draw(&game->ken_sprite, game->engine->renderer);
    elapsed_time_draw(&game->elapsed_time, game->engine->renderer);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
#include "game.h"
#include "assets.h"

#include <stdio.h>

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_renderer_set_clear_color(game->engine->renderer, 0, 0, 255);

    fw64_camera_init(&game->camera);

    fw64_renderer_set_anti_aliasing_enabled(game->engine->renderer, 0);

    n64_logo_sprite_init(&game->n64logo, fw64_image_load(game->engine->assets, FW64_ASSET_image_n64_logo, NULL));
    game->n64logo.position.x = 10;
    game->n64logo.position.y = 10;

    ken_sprite_init(&game->ken_sprite, fw64_image_load(game->engine->assets, FW64_ASSET_image_ken, NULL));

    IVec2 screen_size;
    fw64_renderer_get_screen_size(game->engine->renderer, &screen_size);
    game->ken_sprite.position.x = 10;
    game->ken_sprite.position.y = screen_size.y - 10 - fw64_texture_height(game->ken_sprite.sprite);

    elapsed_time_init(&game->elapsed_time, fw64_font_load(engine->assets, FW64_ASSET_font_basicLAZER, NULL));
    fw64Font* sm64_font = fw64_font_load(engine->assets, FW64_ASSET_font_Mario64, NULL);
    typewriter_text_init(&game->typewriter_text, "super mario 64", sm64_font);

    game->nintendo_seal = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_nintendo_seal, NULL), NULL);
    game->overlay = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_overlay, NULL), NULL);
}

void game_update(Game* game){
    float time_delta = game->engine->time->time_delta;

    ken_sprite_update(&game->ken_sprite, time_delta);
    elapsed_time_update(&game->elapsed_time, time_delta);
    typewriter_text_update(&game->typewriter_text, time_delta);
}

void game_draw(Game* game){
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, &game->camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    n64_logo_sprite_draw(&game->n64logo, renderer);
    ken_sprite_draw(&game->ken_sprite, renderer);
    elapsed_time_draw(&game->elapsed_time, renderer);
    typewriter_text_draw(&game->typewriter_text, renderer);
    fw64_renderer_draw_sprite(renderer, game->nintendo_seal, 275, 200);
    fw64_renderer_draw_sprite(renderer, game->overlay, 275, 200);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}
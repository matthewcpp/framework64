#include "game.h"

#include "framework64/matrix.h"
#include "framework64/util/renderpass_util.h"

#include "assets/assets.h"

#include <stdio.h>

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();

    game->engine = engine;
    game->render_pass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), allocator);
    fw64_renderpass_util_ortho2d(game->render_pass);
    fw64_renderpass_set_clear_color(game->render_pass, 0, 0, 255);

    game->sprite_batch = fw64_spritebatch_create(2, allocator);

    n64_logo_sprite_init(&game->n64logo, fw64_assets_load_image(game->engine->assets, FW64_ASSET_image_n64_logo, allocator));
    game->n64logo.position.x = 10;
    game->n64logo.position.y = 10;

    ken_sprite_init(&game->ken_sprite, fw64_assets_load_image(game->engine->assets, FW64_ASSET_image_ken, allocator));

    IVec2 screen_size = fw64_display_get_size(fw64_displays_get_primary(engine->displays));
    game->ken_sprite.position.x = 10;
    game->ken_sprite.position.y = screen_size.y - 10 - fw64_texture_height(game->ken_sprite.sprite);

    elapsed_time_init(&game->elapsed_time, fw64_assets_load_font(engine->assets, FW64_ASSET_font_basicLAZER, allocator));
    fw64Font* sm64_font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_Mario64, allocator);
    typewriter_text_init(&game->typewriter_text, "super mario 64", sm64_font);

    game->nintendo_seal = fw64_texture_create_from_image(fw64_assets_load_image(engine->assets, FW64_ASSET_image_nintendo_seal, allocator), allocator);
    game->overlay = fw64_texture_create_from_image(fw64_assets_load_image(engine->assets, FW64_ASSET_image_overlay, allocator), allocator);
}

void game_update(Game* game){
    float time_delta = game->engine->time->time_delta;

    ken_sprite_update(&game->ken_sprite, time_delta);
    elapsed_time_update(&game->elapsed_time, time_delta);
    typewriter_text_update(&game->typewriter_text, time_delta);
}

void draw_sprites(Game* game) {
    fw64SpriteBatch* sprite_batch = game->sprite_batch;

    fw64_spritebatch_begin(sprite_batch);

    n64_logo_sprite_draw(&game->n64logo, sprite_batch);
    ken_sprite_draw(&game->ken_sprite, sprite_batch);
    elapsed_time_draw(&game->elapsed_time, sprite_batch);
    typewriter_text_draw(&game->typewriter_text, sprite_batch);

    fw64_spritebatch_set_active_layer(sprite_batch, 0);
    fw64_spritebatch_draw_sprite(sprite_batch, game->nintendo_seal, 275, 200);

    fw64_spritebatch_set_active_layer(sprite_batch, 1);
    fw64_spritebatch_draw_sprite(sprite_batch, game->overlay, 275, 200);

    fw64_spritebatch_end(sprite_batch);
}

void game_draw(Game* game){
    fw64_renderpass_begin(game->render_pass);
    draw_sprites(game);
    fw64_renderpass_draw_sprite_batch(game->render_pass, game->sprite_batch);
    fw64_renderpass_end(game->render_pass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->render_pass);
}

#include "game.h"
#include "assets/assets.h"

#include "framework64/util/renderpass_util.h"
#include "framework64/util/texture_util.h"

#include "framework64/controller_mapping/n64.h"

#define BACKGROUND_MOVIE_COUNT 5
#define BUMP_ALLOCATOR_SIZE ( 64 * 1024)

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_bump_allocator_init(&game->bump_allocator, BUMP_ALLOCATOR_SIZE);
    fw64Allocator* allocator = &game->bump_allocator.interface;
    
    fw64Image* image = fw64_assets_load_image(engine->assets, FW64_ASSET_image_n64_controller, allocator);
    fw64Image* image2 = fw64_assets_load_image(engine->assets, FW64_ASSET_image_pyoro64, allocator);
    fw64Image* image3 = fw64_assets_load_image(engine->assets, FW64_ASSET_image_n64_logo, allocator);

    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpass);

    game->spritebatch = fw64_spritebatch_create(1, allocator);
    game->test_texture1 = fw64_texture_create_from_image(image, allocator);
    game->test_texture2 = fw64_texture_create_from_image(image2, allocator);
    game->test_texture3 = fw64_texture_create_from_image(image3, allocator);

    fw64_renderpass_set_clear_color(game->renderpass, 8, 200, 10);
}

void game_update(Game* game){
    (void)game;
}

void game_fixed_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_spritebatch_begin(game->spritebatch);
    fw64_spritebatch_set_color(game->spritebatch, 0, 0, 255, 255);
    fw64_spritebatch_draw_sprite(game->spritebatch, game->test_texture1, 10, 10);

    fw64_spritebatch_set_color(game->spritebatch, 255, 255, 255, 255);
    fw64_spritebatch_draw_sprite(game->spritebatch, game->test_texture2, 25, 100);

    fw64_spritebatch_set_color(game->spritebatch, 255, 0, 0, 255);
    fw64_spritebatch_draw_sprite(game->spritebatch, game->test_texture1, 40, 40);

    fw64_spritebatch_set_color(game->spritebatch, 255, 255, 255, 255);
    fw64_spritebatch_draw_sprite(game->spritebatch, game->test_texture3, 125, 75);

    // fw64_spritebatch_set_color(game->spritebatch, 100, 0, 75, 255);
    // fw64_spritebatch_draw_sprite(game->spritebatch, game->test_texture1, 10, 85);

    // fw64_spritebatch_set_color(game->spritebatch, 0, 111, 10, 19);
    // fw64_spritebatch_draw_sprite(game->spritebatch, game->test_texture2, 200, 10);

    fw64_spritebatch_end(game->spritebatch);

    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_draw_sprite_batch(game->renderpass, game->spritebatch);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
}


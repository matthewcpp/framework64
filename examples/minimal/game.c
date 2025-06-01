#include "game.h"
#include "assets/assets.h"

#include "framework64/util/renderpass_util.h"
#include "framework64/util/texture_util.h"

#include "framework64/controller_mapping/n64.h"

#define BACKGROUND_MOVIE_COUNT 5


void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64Display* display = fw64_displays_get_primary(engine->displays);
    fw64Allocator* allocator = fw64_default_allocator();

    game->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpass);

    game->spritebatch = fw64_spritebatch_create(1, allocator);

    fw64_spritebatch_begin(game->spritebatch);
    fw64_spritebatch_draw_sprite(game->spritebatch, game->test_texture, 50, 50);
    fw64_spritebatch_end(game->spritebatch);
}

void game_update(Game* game){
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        game->test_texture = fw64_texture_util_create_from_loaded_image(game->engine->assets, FW64_ASSET_image_pyoro64, fw64_default_allocator());
        fw64_renderer_set_clear_color(game->engine->renderer, 0, 255, 0);
    }
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL );
    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_draw_sprite_batch(game->renderpass, game->spritebatch);
    fw64_renderpass_end(game->renderpass);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}


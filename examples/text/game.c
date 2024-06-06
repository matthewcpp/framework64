#include "game.h"
#include "assets/assets.h"

#include "framework64/matrix.h"
#include "framework64/controller_mapping/n64.h"

static void set_active_font(Game* game, int active_font_index);

void game_init(Game* game, fw64Engine* engine) {
    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->engine = engine;

    fw64AssetId fontAssets[FONT_COUNT] = {FW64_ASSET_font_Consolas12, FW64_ASSET_font_basicLAZER, FW64_ASSET_font_Mario64};
    for (size_t i = 0; i < FONT_COUNT; i++) {
        game->fonts[i] = fw64_assets_load_font(engine->assets, fontAssets[i], allocator);
    }

    game->sprite_batch = fw64_spritebatch_create(1, allocator);
    set_active_font(game, 0);

    Vec2 display_size = fw64_display_get_size_f(display);
    float matrix[16];
    matrix_ortho2d(matrix, 0, display_size.x, display_size.y, 0);

    game->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_projection_matrix(game->renderpass, matrix, NULL);
    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_draw_sprite_batch(game->renderpass, game->sprite_batch);
    fw64_renderpass_end(game->renderpass);
}

void set_active_font(Game* game, int active_font_index) {
    game->active_font_index = active_font_index;

    fw64SpriteBatch* sprite_batch = game->sprite_batch;
    fw64Font* font = game->fonts[game->active_font_index];

    fw64_spritebatch_begin(game->sprite_batch);

    int x_pos = 10, y_pos = 10;

    fw64_spritebatch_set_color(sprite_batch, 255, 255, 255, 255);
    fw64_spritebatch_draw_string(sprite_batch, font, "hello world", x_pos, y_pos);
    y_pos += fw64_font_line_height(font);
    
    fw64_spritebatch_set_color(sprite_batch, 0, 255, 0, 255);
    fw64_spritebatch_draw_string(sprite_batch, font, "foo bar baz", x_pos, y_pos);
    y_pos += fw64_font_line_height(font);

    fw64_spritebatch_set_color(sprite_batch, 100, 60, 20, 255);
    fw64_spritebatch_draw_string(sprite_batch, font, "quick brown fox", x_pos, y_pos);

    fw64_spritebatch_end(sprite_batch);
}

static void next_font(Game* game, int direction) {
    int next_index = game->active_font_index + direction;

    if (next_index == FONT_COUNT) {
        next_index = 0;
    } else if (next_index < 0) {
        next_index = FONT_COUNT - 1;
    }

    set_active_font(game, next_index);
}

void game_update(Game* game){
    if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_LEFT)) {
        next_font(game, -1);
    } else if (fw64_input_controller_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        next_font(game, 1);
    }
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

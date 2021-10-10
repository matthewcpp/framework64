#include "game.h"
#include "assets.h"
#include "framework64/util/quad.h"
#include "framework64/n64/controller_button.h"

#include "assets.h"

static void set_texture_mode(Game* game, Mode mode);

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;
    fw64_camera_init(&game->camera);

    fw64_renderer_set_clear_color(engine->renderer, 39, 58, 93);
    game->mode = MODE_DEFAULT;

    game->font = fw64_font_load(engine->assets, FW64_ASSET_font_Consolas12);
    game->buttons = fw64_texture_create_from_image(fw64_image_load(engine->assets, FW64_ASSET_image_buttons));

    fw64_node_init(&game->quad, NULL);
    set_texture_mode(game, MODE_DEFAULT);
}

void game_update(Game* game){
    if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_LEFT) && game->mode > MODE_DEFAULT)
        set_texture_mode(game, game->mode - 1);

    if (fw64_input_button_pressed(game->engine->input, 0, FW64_N64_CONTROLLER_BUTTON_C_RIGHT) && game->mode < MODE_MIRROR)
        set_texture_mode(game, game->mode + 1);
}

void game_draw(Game* game) {
    fw64Renderer* renderer = game->engine->renderer;

    fw64_renderer_begin(renderer, &game->camera, FW64_RENDERER_MODE_TRIANGLES, FW64_RENDERER_FLAG_CLEAR);
    fw64_renderer_draw_static_mesh(renderer, &game->quad.transform, game->quad.mesh);
    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_NOSWAP);

    IVec2 screen_size;
    fw64_renderer_get_screen_size(renderer, &screen_size);
    IVec2 measurement = fw64_font_measure_text(game->font, game->mode_name);
    int slice_width = fw64_texture_slice_width(game->buttons);
    int x_pos = screen_size.x / 2 - measurement.x / 2;

    fw64_renderer_begin(renderer, &game->camera, FW64_RENDERER_MODE_ORTHO2D, FW64_RENDERER_FLAG_NOCLEAR);
    fw64_renderer_draw_text(renderer, game->font, x_pos, 10, game->mode_name);

    if (game->mode > MODE_DEFAULT)
        fw64_renderer_draw_sprite_slice(renderer, game->buttons, 4, x_pos - slice_width - 5, 10);

    if (game->mode < MODE_MIRROR)
        fw64_renderer_draw_sprite_slice(renderer, game->buttons, 5, x_pos + measurement.x + 5, 10);

    fw64_renderer_end(renderer, FW64_RENDERER_FLAG_SWAP);
}

void set_texture_mode(Game* game, Mode mode) {
    game->mode = mode;

    fw64TextureWrapMode wrap_mode;
    Vec2 coords_max;

    switch (game->mode)
    {
        case MODE_DEFAULT:
            game->mode_name = "Default";
            vec2_set(&coords_max, 1.0f, 1.0f);
            wrap_mode = FW64_TEXTURE_WRAP_CLAMP;
            break;

        case MODE_CLAMP:
            game->mode_name = "Clamp";
            vec2_set(&coords_max, 2.0f, 2.0f);
            wrap_mode = FW64_TEXTURE_WRAP_CLAMP;

            break;

        case MODE_WRAP:
            game->mode_name = "Repeat";
            vec2_set(&coords_max, 2.0f, 2.0f);
            wrap_mode = FW64_TEXTURE_WRAP_REPEAT;
            break;

        case MODE_MIRROR:
            game->mode_name = "Mirror";
            vec2_set(&coords_max, 2.0f, 2.0f);
            wrap_mode = FW64_TEXTURE_WRAP_MIRROR;
            break;
    }

    if (game->quad.mesh)
        fw64_mesh_delete(game->quad.mesh);

    fw64_node_init(&game->quad, textured_quad_create_with_params(game->engine, FW64_ASSET_image_pyoro64, coords_max.x, coords_max.y));
    fw64Texture* texture = fw64_material_get_texture(fw64_mesh_get_material_for_primitive(game->quad.mesh, 0));
    fw64_texture_set_wrap_mode(texture, wrap_mode, wrap_mode);
}

#include "game.h"
#include "framework64/asset.h"
#include "framework64/util/quad.h"

#include "assets.h"

#include <malloc.h>

static void set_texture_mode(Game* game, Mode mode);

void game_init(Game* game, System* system) {
    game->system = system;
    camera_init(&game->camera);

    Color c = {39, 58, 93};
    renderer_set_clear_color(system->renderer, &c);
    game->mode = MODE_DEFAULT;

    Mesh* mesh = malloc(sizeof(Mesh));
    textured_quad_create(mesh, assets_get_image(system->assets, ASSET_sprite_pyoro64));
    entity_init(&game->quad_entity, mesh);

    game->font = assets_get_font(system->assets, ASSET_font_Consolas12);
    game->buttons = assets_get_image(system->assets, ASSET_sprite_buttons);

    set_texture_mode(game, MODE_DEFAULT);
}

void game_update(Game* game, float time_delta){
    if (input_button_pressed(game->system->input, 0, L_CBUTTONS) && game->mode > MODE_DEFAULT)
        set_texture_mode(game, game->mode - 1);

    if (input_button_pressed(game->system->input, 0, R_CBUTTONS) && game->mode < MODE_MIRROR)
        set_texture_mode(game, game->mode + 1);
}

void game_draw(Game* game) {
    IVec2 screen_size;
    renderer_get_screen_size(game->system->renderer, &screen_size);
    IVec2 measurement = font_measure_text(game->font, game->mode_name);
    int slice_width = image_sprite_get_slice_width(game->buttons);

    int x_pos = screen_size.x / 2 - measurement.x / 2;
    renderer_begin(game->system->renderer, &game->camera, RENDERER_MODE_TRIANGLES, RENDERER_FLAG_CLEAR);
    renderer_draw_static_mesh(game->system->renderer, &game->quad_entity.transform, game->quad_entity.mesh);
    renderer_draw_text(game->system->renderer, game->font, x_pos, 10, game->mode_name);

    if (game->mode > MODE_DEFAULT)
        renderer_draw_sprite_slice(game->system->renderer, game->buttons, 4, x_pos - slice_width - 5, 10);

    if (game->mode < MODE_MIRROR)
        renderer_draw_sprite_slice(game->system->renderer, game->buttons, 5, x_pos + measurement.x + 5, 10);

    renderer_end(game->system->renderer, RENDERER_FLAG_SWAP);
}

void set_texture_mode(Game* game, Mode mode) {
    ImageSprite* texture = game->quad_entity.mesh->textures;
    game->mode = mode;

    switch (game->mode)
    {
    case MODE_DEFAULT:
        game->mode_name = "Default";
        textured_quad_set_tex_coords(game->quad_entity.mesh, 0, 1.0f, 1.0f);
        texture->wrap_s = G_TX_CLAMP;
        texture->wrap_t = G_TX_CLAMP;
        texture->mask_s = G_TX_NOMASK;
        texture->mask_t = G_TX_NOMASK;
        break;

    case MODE_CLAMP:
        game->mode_name = "Clamp";
        textured_quad_set_tex_coords(game->quad_entity.mesh, 0, 2.0f, 2.0f);
        texture->wrap_s = G_TX_CLAMP;
        texture->wrap_t = G_TX_CLAMP;
        texture->mask_s = G_TX_NOMASK;
        texture->mask_t = G_TX_NOMASK;
        break;

    case MODE_WRAP:
        game->mode_name = "Wrap";
        textured_quad_set_tex_coords(game->quad_entity.mesh, 0, 2.0f, 2.0f);
        texture->wrap_s = G_TX_WRAP;
        texture->wrap_t = G_TX_WRAP;
        texture->mask_s = 4;
        texture->mask_t = 4;
        break;

    case MODE_MIRROR:
        game->mode_name = "Mirror";
        textured_quad_set_tex_coords(game->quad_entity.mesh, 0, 2.0f, 1.0f);
        texture->wrap_s = G_TX_MIRROR;
        texture->wrap_t = G_TX_CLAMP;
        texture->mask_s = 4;
        texture->mask_t = G_TX_NOMASK;
        break;
    }
}

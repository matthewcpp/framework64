#include "game.h"
#include "assets/assets.h"

#include "framework64/controller_mapping/n64.h"
#include "framework64/util/renderpass_util.h"
#include "framework64/util/text_util.h"
#include "framework64/util/texture_util.h"

typedef struct {
    uint32_t x, y, width, height;
    fw64ColorRGBA8 color;
} Element;

void game_init(Game* game, fw64Engine* engine) {
    game->engine = engine;

    fw64Allocator* allocator = fw64_default_allocator();
    fw64Display* display = fw64_displays_get_primary(engine->displays);

    game->spritebatch = fw64_spritebatch_create(2, allocator);
    game->renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_util_ortho2d(game->renderpass);

    fw64_renderer_set_clear_color(engine->renderer, 225, 225, 225);

    game->font = fw64_assets_load_font(engine->assets, FW64_ASSET_font_consolas, allocator);
    game->fill_tex = fw64_texture_util_create_from_loaded_image(engine->assets, FW64_ASSET_image_fill, allocator);

    fw64DataSource* datasource = fw64_assets_open_datasource(engine->assets, FW64_ASSET_file_rby1930);
    char title[64];
    fw64_data_source_read(datasource, title, 1, sizeof(title));
    
    uint32_t element_count;
    fw64_data_source_read(datasource, &element_count, 1, sizeof(element_count));
    Element* elements = fw64_allocator_malloc(allocator, sizeof(Element) * element_count);
    fw64_data_source_read(datasource, elements, sizeof(Element), element_count);
    fw64_assets_close_datasource(engine->assets, datasource);


    const IVec2 area = {fw64_display_get_size(display).x, fw64_font_line_height(game->font)};
    const IVec2 centered_pos = fw64_text_util_center_string(game->font, title, &area);
    const IVec2 offset = {60, 30};

    fw64_spritebatch_begin(game->spritebatch);
    fw64_spritebatch_set_active_layer(game->spritebatch, 0);
    fw64_spritebatch_set_color(game->spritebatch, 0, 0, 0, 255);
    fw64_spritebatch_draw_string(game->spritebatch, game->font, title, centered_pos.x, 10);
    fw64_spritebatch_set_active_layer(game->spritebatch, 1);

    for (uint32_t i = 0; i < element_count; i++) {
        const Element* element = &elements[i];
        fw64_spritebatch_set_color(game->spritebatch, element->color.r, element->color.g, element->color.b, element->color.a);
        fw64_spritebatch_draw_sprite_slice_rect(game->spritebatch, game->fill_tex, 0, offset.x + element->x, offset.y + element->y, element->width, element->height);
    }
    fw64_spritebatch_end(game->spritebatch);

    fw64_renderpass_begin(game->renderpass);
    fw64_renderpass_draw_sprite_batch(game->renderpass, game->spritebatch);
    fw64_renderpass_end(game->renderpass);
}

void game_update(Game* game){
    (void)game;
}

void game_draw(Game* game) {
    fw64_renderer_begin(game->engine->renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL);
    fw64_renderer_submit_renderpass(game->engine->renderer, game->renderpass);
    fw64_renderer_end(game->engine->renderer, FW64_RENDERER_FLAG_SWAP);
}

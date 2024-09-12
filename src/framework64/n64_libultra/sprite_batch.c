#include "framework64/n64_libultra/sprite_batch.h"

#include "framework64/n64_libultra/font.h"
#include "framework64/n64_libultra/image.h"
#include "framework64/n64_libultra/texture.h"
#include "framework64/n64_libultra/vertex.h"

#include <ultra64.h> 
#include <nusys.h>

#include <string.h>

fw64SpriteBatch* fw64_spritebatch_create(size_t layer_count, fw64Allocator* allocator) {
    fw64SpriteBatch* sprite_batch = allocator->malloc(allocator, sizeof(fw64SpriteBatch));
    fw64_n64_texture_batch_init(&sprite_batch->texture_batch, layer_count, allocator);
    fw64_color_rgba8_set(&sprite_batch->color, 255, 255, 255, 255);

    return sprite_batch;
}

void fw64_spritebatch_delete(fw64SpriteBatch* sprite_batch) {
    fw64_n64_texture_batch_uninit(&sprite_batch->texture_batch);
}

void fw64_spritebatch_set_color(fw64SpriteBatch* sprite_batch, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    fw64_color_rgba8_set(&sprite_batch->color, r, g, b, a);
}

size_t fw64_spritebatch_get_layer_count(fw64SpriteBatch* sprite_batch) {
    return sprite_batch->texture_batch.layer_count;
}

int fw64_spritebatch_set_active_layer(fw64SpriteBatch* sprite_batch, size_t layer_index) {
    return fw64_n64_texture_batch_set_active_layer(&sprite_batch->texture_batch, layer_index);
}

void fw64_spritebatch_begin(fw64SpriteBatch* sprite_batch) {
    fw64_n64_texture_batch_reset(&sprite_batch->texture_batch);
}

void fw64_spritebatch_end(fw64SpriteBatch* sprite_batch) {
    fw64_n64_texture_batch_finalize(&sprite_batch->texture_batch);
}

static void fw64_spritebatch_add_quad(fw64SpriteBatch* sprite_batch, fw64Texture* texture, uint16_t index, int16_t draw_pos_x, int16_t draw_pos_y, int16_t tile_width, int16_t tile_height) {
    fw64N64QuadBatch* batch = fw64_n64_texture_batch_get_batch(&sprite_batch->texture_batch, texture, index);
    Vtx* vtx = batch->vertices + (batch->count * 4);

    fw64_n64_vertex_set_position_s16(vtx, draw_pos_x, draw_pos_y + tile_height, 0);
    fw64_n64_vertex_set_color(vtx, sprite_batch->color);
    fw64_n64_vertex_set_texcoords_f(vtx++, texture, 0.0f, 1.0f);

    fw64_n64_vertex_set_position_s16(vtx, draw_pos_x + tile_width, draw_pos_y + tile_height, 0);
    fw64_n64_vertex_set_color(vtx, sprite_batch->color);
    fw64_n64_vertex_set_texcoords_f(vtx++, texture, 1.0f, 1.0f);

    fw64_n64_vertex_set_position_s16(vtx, draw_pos_x + tile_width, draw_pos_y, 0);
    fw64_n64_vertex_set_color(vtx, sprite_batch->color);
    fw64_n64_vertex_set_texcoords_f(vtx++, texture, 1.0f, 0.0f);

    fw64_n64_vertex_set_position_s16(vtx, draw_pos_x, draw_pos_y, 0);
    fw64_n64_vertex_set_color(vtx, sprite_batch->color);
    fw64_n64_vertex_set_texcoords_f(vtx++, texture, 0.0f, 0.0f);

    batch->count += 1;
}

void fw64_spritebatch_draw_sprite(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int x, int y) {
    int slice_width = fw64_texture_slice_width(texture);
    int slice_height = fw64_texture_slice_height(texture);
    int slice = 0;

    for (uint8_t row = 0; row < texture->image->info.vslices; row++ ) {
        int draw_y = y + row * slice_height;
        for (uint8_t col = 0; col < texture->image->info.hslices; col++) {
            int draw_x = x + slice_width * col;

            fw64_spritebatch_add_quad(sprite_batch, texture, slice++, draw_x, draw_y, slice_width, slice_height);
        }
    }
}

void fw64_spritebatch_draw_sprite_slice(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int frame, int x, int y) {
    fw64_spritebatch_add_quad(sprite_batch, texture, frame, x, y, fw64_texture_slice_width(texture), fw64_texture_slice_height(texture));
}

void fw64_spritebatch_draw_sprite_slice_rect(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int frame, int x, int y, int width, int height) {
    fw64_spritebatch_add_quad(sprite_batch, texture, frame, x, y, width, height);
}

void fw64_spritebatch_draw_string(fw64SpriteBatch* sprite_batch, fw64Font* font, const char* text, int x, int y) {
    fw64_spritebatch_draw_string_count(sprite_batch, font, text, UINT32_MAX, x, y);
}

void fw64_spritebatch_draw_string_count(fw64SpriteBatch* sprite_batch, fw64Font* font, const char* text, uint32_t count, int x, int y) {
    if (!text || text[0] == 0) return;
    
    char ch = text[0];
    uint16_t glyph_index = fw64_n64_font_get_glyph_index(font, ch);
    fw64FontGlyph* glyph = font->glyphs + glyph_index;
    fw64Texture* texture = &font->texture;
    int tile_width = fw64_texture_slice_width(texture);
    int tile_height = fw64_texture_slice_height(texture);

    int caret = x + glyph->left;

    for (uint32_t i = 0; i < count && ch != 0; i++) {
        glyph_index = fw64_n64_font_get_glyph_index(font, ch);
        glyph = font->glyphs + glyph_index;

        int draw_pos_x = caret + glyph->left;
        int draw_pos_y = y + glyph->top;

        fw64_spritebatch_add_quad(sprite_batch, texture, glyph_index, draw_pos_x, draw_pos_y, tile_width, tile_height);

        caret += glyph->advance;
        text++;
        ch = text[0];
    }
}

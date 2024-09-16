#include "framework64/n64_libultra/font.h"

#include "framework64/n64_libultra/asset_database.h"
#include "framework64/n64_libultra/image.h"

#include "framework64/filesystem.h"
#include "framework64/vec2.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

fw64Font* fw64_font_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator) {
    fw64N64FontInfo font_info;
    fw64_data_source_read(data_source, &font_info, sizeof(fw64N64FontInfo), 1);

    fw64Font* font = fw64_allocator_malloc(allocator, sizeof(fw64Font));
    font->glyph_count = (int)font_info.glyph_count;
    font->size = (int)font_info.size;
    font->line_height = (int)font_info.line_height;

    size_t glyph_data_size = sizeof(fw64FontGlyph) * font->glyph_count;
    font->glyphs = fw64_allocator_malloc(allocator, glyph_data_size);
    fw64_data_source_read(data_source, font->glyphs, 1, glyph_data_size);
    

    // load the font's image
    fw64Image* image = fw64_allocator_malloc(allocator, sizeof(fw64Image));
    fw64_n64_image_read_data(image, data_source, allocator);
    fw64_n64_texture_init_with_image(&font->texture, image);

    return font;
}

void fw64_font_delete(fw64AssetDatabase* assets, fw64Font* font, fw64Allocator* allocator) {
    if (!allocator) allocator = fw64_default_allocator();
    fw64_allocator_free(allocator, font->glyphs);
    fw64_image_delete(assets, font->texture.image, allocator);
    fw64_allocator_free(allocator, font);
}

static uint16_t find_font_glyph_rec(fw64FontGlyph* glyphs, int min_index, int max_index, uint16_t codepoint) {
    uint32_t center_index = min_index + (max_index - min_index) / 2;
    if (min_index > max_index) return UINT16_MAX;

    fw64FontGlyph* center_glyph = glyphs + center_index;

    if (center_glyph->codepoint == codepoint) {
        return center_index;
    }
    else if (center_glyph->codepoint > codepoint) {
        return find_font_glyph_rec(glyphs, min_index, center_index - 1, codepoint);
    }
    else if (center_glyph->codepoint < codepoint) {
        return find_font_glyph_rec(glyphs, center_index + 1, max_index, codepoint);
    }
    else {
        return center_index;
    }
}

uint16_t fw64_n64_font_get_glyph_index(fw64Font* font, uint16_t codepoint) {
    return find_font_glyph_rec(font->glyphs, 0, font->glyph_count - 1, codepoint);
}

IVec2 fw64_font_measure_text(fw64Font* font, const char* text) {
    return fw64_font_measure_text_len(font, text, UINT32_MAX);
}

IVec2 fw64_font_measure_text_len(fw64Font* font, const char* text, size_t len) {
    IVec2 measurement = {0, 0};
     if (!text || text[0] == 0 || len == 0) return measurement;

    fw64FontGlyph* glyph = font->glyphs + fw64_n64_font_get_glyph_index(font, text[0]);
    measurement.x = -glyph->left;

    for (size_t i = 0; i < len; i++) {
        int bottom = glyph->top + glyph->height;

        if (bottom > measurement.y)
            measurement.y = bottom;

        measurement.x += glyph->advance;
        
        text++;
        if (text[0] == 0) {
            break;
        }
        glyph = font->glyphs + fw64_n64_font_get_glyph_index(font, text[0]);
    }

    return measurement;
}

int fw64_font_size(fw64Font* font) {
    return (int)font->size;
}

int fw64_font_line_height(fw64Font* font) {
    return font->line_height;
}

uint32_t fw64_font_get_glyph_count(fw64Font* font) {
    return font->glyph_count;
}

fw64FontGlyph* fw64_font_get_glyph_by_index(fw64Font* font, size_t glyph_index) {
    return font->glyphs + glyph_index;
}

uint32_t fw64_font_get_glyph_index_for_codepoint(fw64Font* font, fw64FontCodepoint codepoint) {
    uint16_t glyph_index = fw64_n64_font_get_glyph_index(font, codepoint);

    return font->glyphs[glyph_index].codepoint == codepoint ? glyph_index : fw64FontInvalidCodepointIndex;
}

fw64FontCodepoint fw64_font_glyph_codepoint(fw64FontGlyph* glyph) {
    return (fw64FontCodepoint)glyph->codepoint;
}
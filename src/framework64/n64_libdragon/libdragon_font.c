#include "libdragon_font.h"

// this needs to be kept in sync with pipeline/n64_libdragon/FontWriter.js
fw64Font* fw64_font_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator) {
    fw64Font* font = fw64_allocator_malloc(allocator, sizeof(fw64Font));

    fw64_data_source_read(data_source, &font->info, sizeof(fw64LibDragonFontInfo), 1);
    
    // TODO: this should probably be updated when textures are fleshed out
    font->glyph_texture.image = fw64_allocator_malloc(allocator, sizeof(fw64Image));
    if (!fw64_libdragon_image_init_from_datasource(font->glyph_texture.image, data_source, allocator)) {
        fw64_allocator_free(allocator, font);
        return NULL;
    }

    font->glyphs = fw64_allocator_malloc(allocator, sizeof(fw64LibDragonFontGlyph) * font->info.glyph_count);
    fw64_data_source_read(data_source, font->glyphs, sizeof(fw64LibDragonFontGlyph), font->info.glyph_count);

    return font;
}

void fw64_font_delete(fw64AssetDatabase* assets, fw64Font* font, fw64Allocator* allocator) {
    (void)assets;

    fw64_libdragon_image_uninit(font->glyph_texture.image, allocator);
    fw64_allocator_free(allocator, font->glyph_texture.image);
    fw64_allocator_free(allocator, font->glyphs);
    fw64_allocator_free(allocator, font);
}

int fw64_font_size(fw64Font* font) {
    return font->info.size;
}

int fw64_font_line_height(fw64Font* font) {
    return font->info.line_height;
}

static uint16_t find_font_glyph_rec(fw64LibDragonFontGlyph* glyphs, int min_index, int max_index, uint32_t codepoint) {
    uint32_t center_index = min_index + (max_index - min_index) / 2;
    if (min_index > max_index) return UINT16_MAX;

    fw64LibDragonFontGlyph* center_glyph = glyphs + center_index;

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

uint16_t fw64_libdragon_font_get_glyph_index(fw64Font* font, uint32_t codepoint) {
    return find_font_glyph_rec(font->glyphs, 0, font->info.glyph_count - 1, codepoint);
}

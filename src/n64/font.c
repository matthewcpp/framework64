#include "framework64/font.h"

#include "framework64/filesystem.h"
#include "framework64/rect.h"
#include "framework64/vec2.h"

#include <stdlib.h>
#include <malloc.h>
#include <string.h>

uint16_t find_font_glyph_rec(FontGlyph* glyphs, int min_index, int max_index, uint16_t codepoint) {
    uint32_t center_index = min_index + (max_index - min_index) / 2;
    if (min_index > max_index) return UINT16_MAX;

    FontGlyph* center_glyph = glyphs + center_index;

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

uint16_t font_get_glyph_index(Font* font, uint16_t codepoint) {
    return find_font_glyph_rec(font->glyphs, 0, font->glyph_count - 1, codepoint);
}

IVec2 font_measure_text(Font* font, char* str) {
    IVec2 measurement = {0, 0};
    FontGlyph* glyph = font->glyphs + font_get_glyph_index(font, str[0]);
    measurement.x = -glyph->left;

    do {
        int bottom = glyph->top + glyph->height;

        if (bottom > measurement.y)
            measurement.y = bottom;

        measurement.x += glyph->advance;
        
        str ++;
        glyph = font->glyphs + font_get_glyph_index(font, str[0]);
    } while (str[0] != '\0');

    return measurement;
}

int font_load(int index, Font* font) {
    int handle = filesystem_open(index);

    if (handle == FW64_FILESYSTEM_INVALID_HANDLE)
        return 0;

    filesystem_read(font, sizeof(uint16_t), 4, handle);

    uint32_t spritefont_data_size = (font->spritefont_tile_width * font->spritefont_tile_height * 2) * font->glyph_count;
    uint32_t glyph_data_size = sizeof(FontGlyph) * font->glyph_count;

    uint8_t* payload = memalign(8, spritefont_data_size + glyph_data_size);
    filesystem_read(payload, 1, spritefont_data_size + glyph_data_size, handle);
    filesystem_close(handle);
    
    font->spritefont = payload;
    font->glyphs = (FontGlyph*)(payload + spritefont_data_size);

    return 1;
}

void font_unload(Font* font) {
    free(font->spritefont);
}
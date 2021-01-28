#include "ultra/font.h"

#include "ultra/rect.h"
#include "ultra/vec2.h"

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
        
        str += 1;
        glyph = font->glyphs + font_get_glyph_index(font, str[0]);
    } while (str[0] != '\0');

    return measurement;
}

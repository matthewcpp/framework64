#ifndef FW64_FONT_H
#define FW64_FONT_H

#include "framework64/vec2.h"

#include <stdint.h>

typedef struct {
    uint16_t codepoint;
    int8_t top;
    int8_t left;
    int8_t advance;
    int8_t height;
} FontGlyph;

typedef struct {
    uint16_t size;
    uint16_t glyph_count;
    uint16_t spritefont_tile_width;
    uint16_t spritefont_tile_height;
    FontGlyph* glyphs;
    uint8_t* spritefont;
} Font;


uint16_t font_get_glyph_index(Font* font, uint16_t codepoint);

IVec2 font_measure_text(Font* font, const char* text);

int font_load(int index, Font* font);
void font_unload(Font* font);

#endif
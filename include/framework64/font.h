#ifndef FONT_H
#define FONT_H

#include "framework64/sprite.h"

#include <stdint.h>

typedef struct {
    uint16_t codepoint;
    int8_t top;
    int8_t left;
    int8_t advance;
    int8_t height;
} FontGlyph;

typedef struct {
    FontGlyph* glyphs;
    uint16_t glyph_count;
    uint16_t size;
    uint16_t spritefont_tile_width;
    uint16_t spritefont_tile_height;
    uint8_t* spritefont;
} Font;


uint16_t font_get_glyph_index(Font* font, uint16_t codepoint);

IVec2 font_measure_text(Font* font, char* text);

#endif
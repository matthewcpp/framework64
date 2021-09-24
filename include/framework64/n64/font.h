#pragma once

#include "framework64/font.h"

typedef struct {
    uint16_t codepoint;
    int8_t top;
    int8_t left;
    int8_t advance;
    int8_t height;
} fw64FontGlyph;

struct fw64Font {
    uint16_t size;
    uint16_t glyph_count;
    uint16_t spritefont_tile_width;
    uint16_t spritefont_tile_height;
    fw64FontGlyph* glyphs;
    uint8_t* spritefont;
};

uint16_t fw64_font_get_glyph_index(fw64Font* font, uint16_t codepoint);

void fw64_font_unload(fw64Font* font);

#pragma once

#include "framework64/font.h"
#include "framework64/n64/texture.h"

struct fw64FontGlyph{
    uint32_t codepoint;
    int8_t top;
    int8_t left;
    int8_t advance;
    int8_t height;
};

typedef struct {
    uint16_t size;
    uint16_t glyph_count;
} fw64N64FontInfo;

struct fw64Font {
    uint16_t size;
    uint16_t glyph_count;
    fw64Texture texture;
    fw64FontGlyph* glyphs;
};

uint16_t fw64_n64_font_get_glyph_index(fw64Font* font, uint16_t codepoint);

#pragma once

#include "framework64/font.h"
#include "libdragon_vertex.h"
#include "libdragon_image.h"
#include "libdragon_texture.h"

typedef struct {
    uint32_t size;
    uint32_t line_height;
    uint32_t glyph_count;
    uint32_t glyphs_per_slice;
} fw64LibDragonFontInfo;

struct fw64FontGlyph {
    uint32_t codepoint;
    uint16_t frame;
    uint16_t index;
    int8_t top;
    int8_t left;
    int8_t advance;
    int8_t height;
};

struct fw64Font {
    fw64LibDragonFontInfo info;
    fw64Texture glyph_texture;
    fw64LibdragonTexCoord* textcoord_lookup;
    fw64FontGlyph* glyphs;
};

uint16_t fw64_libdragon_font_get_glyph_index(fw64Font* font, uint32_t codepoint);

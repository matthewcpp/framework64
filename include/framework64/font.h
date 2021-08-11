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
} fw64FontGlyph;

typedef struct {
    uint16_t size;
    uint16_t glyph_count;
    uint16_t spritefont_tile_width;
    uint16_t spritefont_tile_height;
    fw64FontGlyph* glyphs;
    uint8_t* spritefont;
} fw64Font;


uint16_t fw64_font_get_glyph_index(fw64Font* font, uint16_t codepoint);

IVec2 fw64_font_measure_text(fw64Font* font, const char* text);

int fw64_font_load(int index, fw64Font* font);
void fw64_font_unload(fw64Font* font);

#endif
#ifndef FONT_H
#define FONT_H

#include "ultra/sprite.h"

#include <stdint.h>

typedef struct {
    uint16_t codepoint;
    int8_t top;
    int8_t left;
    int8_t advance;
    uint8_t width;
    uint8_t height;
    uint16_t x;
    uint16_t y;
} FontGlyph;

typedef struct {
    FontGlyph* glyphs;
    uint16_t glyph_count;
    uint16_t size;
    uint16_t spritefont_width;
    uint16_t spritefont_height;
    uint8_t* spritefont;
} Font;

typedef struct {
    uint16_t text_width;
    uint16_t text_height;
    uint16_t allocated_width;
    uint8_t* data;
} TextSprite;

TextSprite* font_create_text_sprite(Font* font, char* text);

void text_sprite_uninit(TextSprite* text_sprite); 

#endif
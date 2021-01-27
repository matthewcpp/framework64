#include "ultra/font.h"

#include "ultra/rect.h"
#include "ultra/vec2.h"

#include <stdlib.h>
#include <malloc.h>
#include <string.h>

static FontGlyph* find_font_glyph_rec(FontGlyph* glyphs, int min_index, int max_index, uint16_t codepoint) {
    uint32_t center_index = min_index + (max_index - min_index) / 2;
    if (min_index > max_index) return NULL;

    FontGlyph* center_glyph = glyphs + center_index;

    if (center_glyph->codepoint == codepoint) {
        return center_glyph;
    }
    else if (center_glyph->codepoint > codepoint) {
        return find_font_glyph_rec(glyphs, min_index, center_index - 1, codepoint);
    }
    else if (center_glyph->codepoint < codepoint) {
        return find_font_glyph_rec(glyphs, center_index + 1, max_index, codepoint);
    }
    else {
        return center_glyph;
    }
}

static FontGlyph* find_font_glyph(FontGlyph* glyphs, uint32_t glyph_count, uint16_t codepoint) {
    return find_font_glyph_rec(glyphs, 0, glyph_count - 1, codepoint);
}

static IVec2 font_measure_text(FontGlyph* glyphs, uint32_t glyph_count, char* str) {
    IVec2 measurement = {0, 0};
    FontGlyph* glyph = find_font_glyph(glyphs, glyph_count, str[0]);
    measurement.x = -glyph->left;

    do {
        
        int bottom = glyph->top + glyph->height;

        if (bottom > measurement.y)
            measurement.y = bottom;

        measurement.x += glyph->advance;
        
        str += 1;
        glyph = find_font_glyph(glyphs, glyph_count, str[0]);
    } while (str[0] != '\0');

    return measurement;
}

static void blit_glyph(int caret, unsigned char* dest_data, uint16_t dest_data_width, FontGlyph* glyph, unsigned char* src_data, uint16_t src_data_width) {
    for (int y = 0; y < glyph->height; y++) {
        int dest_index = ((glyph->top + y) * dest_data_width + (caret + glyph->left)) * 2;
        int src_index = ((glyph->y + y) * src_data_width + glyph->x) * 2;
        memcpy(dest_data + dest_index, src_data + src_index, glyph->width * 2);
    }
}

//https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
uint32_t next_power_of_2(uint32_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return ++v;
}

TextSprite* font_create_text_sprite(Font* font, char* text) {
    
    if (!text || text[0] == 0) 
        return NULL;
    
    TextSprite* text_sprite = malloc(sizeof(TextSprite));

    IVec2 measurement = font_measure_text(font->glyphs, font->glyph_count, text);
    text_sprite->text_width = measurement.x;
    text_sprite->text_height = measurement.y;
    text_sprite->allocated_width = next_power_of_2(text_sprite->text_width);

    int slice_size = (text_sprite->allocated_width * text_sprite->text_height) * 2;
    text_sprite->data = memalign(8, slice_size);
    memset(text_sprite->data, 0 , slice_size);

    char ch = text[0];
    FontGlyph* glyph = find_font_glyph(font->glyphs, font->glyph_count, ch);
    int caret = 0;
    caret += -glyph->left;

    while (ch) {
        glyph = find_font_glyph(font->glyphs, font->glyph_count, ch);

        blit_glyph(caret, text_sprite->data, text_sprite->allocated_width, glyph, font->spritefont, font->spritefont_width);

        caret += glyph->advance;
        text++;
        ch = text[0];
    }

    return text_sprite;
}

void text_sprite_uninit(TextSprite* text_sprite) {
    free(text_sprite->data);
}

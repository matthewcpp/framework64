#pragma once

#include "framework64/font.h"
#include "framework64/vec2.h"
#include "framework64/desktop/texture.h"

#include <limits>
#include <memory>
#include <vector>

struct fw64FontGlyph {
    constexpr static uint32_t InvalidIndex = std::numeric_limits<uint32_t>::max();

    uint32_t codepoint;
    float top;
    float left;
    float advance;
    float height;
};

struct fw64Font {
    std::unique_ptr<fw64Texture> texture;
    std::vector<fw64FontGlyph> glyphs;
    int size;

    uint32_t getGlyphIndex(uint32_t codepoint) const;

    /**
     * Measures the width and height (in screen pixels) of a text string.
     */
    Vec2 measureString(const char* text) const;

    /**
     * Gets the glyph index for the first codepoint in a string.  If there is no glyph present for the string then index 0 (MISSING CHARACTER) is returned.
     * @param text input string
     * @param glyph_index out: the index of the glyph for the first codepoint in the string
     * @return pointer to the string after the next codepoint has been consumed
     */
    const char* getNextGlyphIndex(const char* text, uint32_t& glyph_index) const;
};
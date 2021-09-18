#include "framework64/desktop/font.h"

#include <algorithm>
#include <cmath>

uint32_t fw64Font::getGlyphIndex(uint32_t codepoint) const {
    auto result = std::lower_bound(glyphs.begin(), glyphs.end(), codepoint, [](fw64FontGlyph const & glyph, uint32_t codepoint){
        return glyph.codepoint < codepoint;
    });

    if (result == glyphs.end())
        return fw64FontGlyph::InvalidIndex;

    if ((*result).codepoint == codepoint)
        return result - glyphs.begin();
    else
        return fw64FontGlyph::InvalidIndex;
}

Vec2 fw64Font::measureString(const char* text) const{
    Vec2 measure = {0,0};
    if (!text || text[0] == 0) return measure;

    uint32_t glyph_index;
    text = getNextGlyphIndex(text, glyph_index);
    auto const & first_glyph = glyphs[glyph_index];
    measure.x += -first_glyph.left;

    for (;;) {
        auto const & glyph = glyphs[glyph_index];
        measure.x += glyph.advance;

        float bottom = glyph.top + glyph.height;
        measure.y = std::max(measure.y, bottom);

        if (text[0] == 0)
            break;

        text = getNextGlyphIndex(text, glyph_index);
    }


    return measure;
}

// TODO: This needs to handle multi-byte characters.  At that point probably needs to handle malformed strings as well.
const char* fw64Font::getNextGlyphIndex(const char* text, uint32_t& glyph_index) const {
    auto codepoint = static_cast<uint32_t>(text[0]);
    auto codepoint_index = getGlyphIndex(codepoint);
    glyph_index = codepoint_index != fw64FontGlyph::InvalidIndex ? codepoint_index : 0;

    return ++text;
}


// C interface

IVec2 fw64_font_measure_text(fw64Font* font, const char* text) {
    Vec2 measurement = font->measureString(text);

    IVec2 result;
    result.x = static_cast<int>(std::ceilf(measurement.x));
    result.y = static_cast<int>(std::ceilf(measurement.y));

    return result;
}
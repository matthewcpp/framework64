#include "framework64/desktop/font.hpp"

#include "framework64/desktop/asset_database.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

// this struct corresponds to FontHeader in pipeline/desktop/FontWriter.js
struct FontHeader {
    uint32_t size;
    uint32_t glyph_count;
};

fw64Font* fw64Font::loadFromDatasource(fw64DataSource* data_source, fw64Allocator* allocator) {
    assert(allocator != nullptr);
    FontHeader font_header;
    fw64_data_source_read(data_source, &font_header, sizeof(FontHeader), 1);

    auto font = std::make_unique<fw64Font>();
    font->size = font_header.size;
    font->glyphs.resize(font_header.glyph_count);
    fw64_data_source_read(data_source, font->glyphs.data(), sizeof(fw64FontGlyph), font_header.glyph_count);

    auto* image = fw64_image_load_from_datasource(data_source, allocator);
    font->texture = std::make_unique<fw64Texture>(image); // does this texture need to be a pointer?

    return font.release();
}

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

fw64Font* fw64_font_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator) {
    return fw64Font::loadFromDatasource(data_source, allocator);
}

IVec2 fw64_font_measure_text(fw64Font* font, const char* text) {
    Vec2 measurement = font->measureString(text);

    IVec2 result;
    result.x = static_cast<int>(std::ceil(measurement.x));
    result.y = static_cast<int>(std::ceil(measurement.y));

    return result;
}

void fw64_font_delete(fw64AssetDatabase* assets, fw64Font* font, fw64Allocator* allocator) {
    delete font;
}

int fw64_font_size(fw64Font* font) {
    return font->size;
}

size_t fw64_font_get_glyph_count(fw64Font* font) {
    return font->glyphs.size();
}

fw64FontCodepoint fw64_font_get_glyph_codepoint(fw64Font* font, size_t glpyh_index) {
    return font->glyphs[glpyh_index].codepoint;
}
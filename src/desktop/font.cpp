#include "framework64/desktop/font.h"

#include "framework64/desktop/asset_database.h"

#include <algorithm>
#include <cmath>

fw64Font* fw64Font::loadFromDatabase(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->select_font_statement);
    sqlite3_bind_int(database->select_font_statement, 1, index);

    if(sqlite3_step(database->select_font_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_font_statement, 0));
    const std::string texture_path = database->asset_dir + asset_path;
    auto image =  fw64Image::loadImageFile(texture_path);

    if (!image)
        return nullptr;

    auto font = new fw64Font();
    font->texture = std::make_unique<fw64Texture>(image);
    font->size = sqlite3_column_int(database->select_font_statement, 1);

    int tile_width = sqlite3_column_int(database->select_font_statement, 2);
    int tile_height = sqlite3_column_int(database->select_font_statement, 3);

    font->texture->image->hslices = font->texture->image->width / tile_width;
    font->texture->image->vslices = font->texture->image->height / tile_height;

    int glyphCount = sqlite3_column_int(database->select_font_statement, 4);
    font->glyphs.resize(glyphCount);
    memcpy(font->glyphs.data(), sqlite3_column_blob(database->select_font_statement, 5), glyphCount * sizeof(fw64FontGlyph));

    return font;
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

fw64Font * fw64_font_load(fw64AssetDatabase* assets, uint32_t index, fw64Allocator* allocator) {
    return fw64Font::loadFromDatabase(assets, index);
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

#pragma once

/** \file font.h */

#include "framework64/allocator.h"
#include "framework64/data_io.h"
#include "framework64/vec2.h"

#include "framework64/asset_database.h"

#include <limits.h>
#include <stdint.h>

typedef struct fw64Font fw64Font;
typedef struct fw64FontGlyph fw64FontGlyph;
typedef uint32_t fw64FontCodepoint;
#define fw64FontInvalidCodepointIndex UINT32_MAX

#ifdef __cplusplus
extern "C" {
#endif

/** loads a font from a generic datasource. */
fw64Font* fw64_font_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator);

/** Frees all resources used by and deletes a font loaded with \ref fw64_font_load. */
void fw64_font_delete(fw64AssetDatabase* assets, fw64Font* font, fw64Allocator* allocator);

/** Returns the size of the font specified during authoring time.
 * If a font was based on an image, this value will indicate the vertical frame size of the texture
 * */
int fw64_font_size(fw64Font* font);

/** Determines the width and height of a string in screen pixels. Note: This function is not currently setup to work with multiline text. */
IVec2 fw64_font_measure_text(fw64Font* font, const char* text);

/** Determines the width and height of the first number of characters of string in screen pixels. */
IVec2 fw64_font_measure_text_len(fw64Font* font, const char* text, size_t len);

/** Gets the number of glyphs contained in the font */
uint32_t fw64_font_get_glyph_count(fw64Font* font);

/** Returns the glyph at the given index in the font's internal glyph array. */
fw64FontGlyph* fw64_font_get_glyph_by_index(fw64Font* font, size_t glyph_index);

/** Returns the index into the font's internal glyph array for the codepoint. If no glyph exists returns fw64FontInvalidCodepointIndex.*/
uint32_t fw64_font_get_glyph_index_for_codepoint(fw64Font* font, fw64FontCodepoint codepoint);

/** Gets the codepoint for the glyph*/
fw64FontCodepoint fw64_font_glyph_codepoint(fw64FontGlyph* glyph);

#ifdef __cplusplus
}
#endif

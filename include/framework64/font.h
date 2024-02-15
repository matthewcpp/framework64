#pragma once

/** \file font.h */

#include "framework64/allocator.h"
#include "framework64/data_io.h"
#include "framework64/vec2.h"

#include "framework64/asset_database.h"

#include <stdint.h>

typedef struct fw64Font fw64Font;
typedef uint32_t fw64FontCodepoint;

#ifdef __cplusplus
extern "C" {
#endif

/** loads a font from a generic datasource. */
fw64Font* fw64_font_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator);

/** Frees all resources used by and deletes a font loaded with \ref fw64_font_load. */
void fw64_font_delete(fw64AssetDatabase* assets, fw64Font* font, fw64Allocator* allocator);

/** Determines the width and height of a string in screen pixels. */
IVec2 fw64_font_measure_text(fw64Font* font, const char* text);

/** Gets the number of glpys contained in the font */
size_t fw64_font_get_glyph_count(fw64Font* font);

/** Gets the codepoint for the glyph at the given index*/
fw64FontCodepoint fw64_font_get_glyph_codepoint(fw64Font* font, size_t glpyh_index);

/** Returns the size of the font specified during authoring time.
 * If a font was based on an image, this value will indicate the vertical frame size of the texture
 * */
int fw64_font_size(fw64Font* font);

#ifdef __cplusplus
}
#endif

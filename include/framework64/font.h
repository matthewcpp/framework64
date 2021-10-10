#pragma once

/** \file font.h */

#include "framework64/asset_database.h"
#include "framework64/vec2.h"

#include <stdint.h>

typedef struct fw64Font fw64Font;

#ifdef __cplusplus
extern "C" {
#endif

fw64Font* fw64_font_load(fw64AssetDatabase* assets, uint32_t index);
IVec2 fw64_font_measure_text(fw64Font* font, const char* text);

#ifdef __cplusplus
}
#endif

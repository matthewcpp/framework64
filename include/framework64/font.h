#ifndef FW64_FONT_H
#define FW64_FONT_H

#include "framework64/vec2.h"

#include <stdint.h>

typedef struct fw64Font fw64Font;

#ifdef __cplusplus
extern "C" {
#endif

IVec2 fw64_font_measure_text(fw64Font* font, const char* text);

#ifdef __cplusplus
}
#endif

#endif
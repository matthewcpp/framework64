#pragma once

/** \file display.h */

#include "framework64/vec2.h"

typedef struct fw64Display fw64Display;
typedef struct fw64Displays fw64Displays;

#ifdef __cplusplus
extern "C" {
#endif

fw64Display* fw64_displays_get_primary(fw64Displays* displays);

IVec2 fw64_display_get_size(fw64Display* display);

#ifdef __cplusplus
}
#endif
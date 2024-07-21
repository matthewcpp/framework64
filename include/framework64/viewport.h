#pragma once

#include "framework64/vec2.h"

typedef struct fw64Display fw64Display;

/** \file viewport.h */

/** Do not write to this structure directly, instead use \ref fw64_viewport_set or \ref fw64_viewport_set_relative */
typedef struct {
    IVec2 position;
    IVec2 size;
} fw64Viewport;

#ifdef __cplusplus
extern "C" {
#endif

/** Viewport's position and size in window space.  Note: Top Left is 0,0 */
void fw64_viewport_set(fw64Viewport* viewport, const IVec2* position, const IVec2* size);

/** Set Viewport rect proportional to the screen. position and size values should be in the range [0, 1]*/
void fw64_viewport_set_relative_to_display(fw64Viewport* viewport, fw64Display* display, Vec2* position, Vec2* size);

#ifdef __cplusplus
}
#endif


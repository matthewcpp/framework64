#pragma once

/** \file rect.h */

#include "framework64/vec2.h"

typedef struct {
    /** The center of the rectangle */
    Vec2 origin;

    /** The extents of the rectangle */
    Vec2 size;
} fw64Rect;

/// Returns a non zero value if Rect a contains Rect b
int fw64_rect_contains_rect(fw64Rect* a, fw64Rect* b);

#define fw64_rect_min_x(rect) ((rect)->origin.x - (rect)->size.x)
#define fw64_rect_max_x(rect) ((rect)->origin.x + (rect)->size.x)

#define fw64_rect_min_y(rect) ((rect)->origin.y - (rect)->size.y)
#define fw64_rect_max_y(rect) ((rect)->origin.y + (rect)->size.y)

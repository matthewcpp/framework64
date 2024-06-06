#include "framework64/n64_libultra/display.h"


fw64Display* fw64_displays_get_primary(fw64Displays* displays) {
    return &displays->primary;
}

IVec2 fw64_display_get_size(fw64Display* display) {
    return display->size;
}

Vec2 fw64_display_get_size_f(fw64Display* display) {
    Vec2 result;

    result.x = (float)display->size.x;
    result.y = (float)display->size.y;

    return result;
}

void fw64_n64_libultra_displays_init(fw64Displays* displays, int width, int height) {
    ivec2_set(&displays->primary.size, width, height);
}
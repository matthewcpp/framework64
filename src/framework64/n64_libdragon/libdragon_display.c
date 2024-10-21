#include "libdragon_display.h"

#include <libdragon.h>

void fw64_n64_libdragon_displays_init(fw64Displays* displays) {
    displays->primary.size.x = 320;
    displays->primary.size.y = 240;

    // temp used for video display
    displays->fps = 30.0f;
}

fw64Display* fw64_displays_get_primary(fw64Displays* displays) {
    return &displays->primary;
}

IVec2 fw64_display_get_size(fw64Display* display) {
    return display->size;
}

Vec2 fw64_display_get_size_f(fw64Display* display) {
    Vec2 vf = {(float)display->size.x, (float)display->size.y};

    return vf;
}

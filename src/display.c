#include "framework64/display.h"

struct fw64Display{
    IVec2 size;
};

struct fw64Displays{
    fw64Display primary;
};

fw64Display* fw64_displays_get_primary(fw64Displays* displays) {
    return &displays->primary;
}

IVec2 fw64_display_get_size(fw64Display* display) {
    return display->size;
}
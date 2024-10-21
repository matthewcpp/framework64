#pragma once

#include "framework64/display.h"

struct fw64Display {
    IVec2 size;
};

struct fw64Displays {
    fw64Display primary;
    float fps;
};

void fw64_n64_libdragon_displays_init(fw64Displays* displays);

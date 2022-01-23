#pragma once

#include "framework64/framebuffer.h"

#include <nusys.h>

struct fw64Framebuffer {
    u16* buffer;
    int width, height;
};
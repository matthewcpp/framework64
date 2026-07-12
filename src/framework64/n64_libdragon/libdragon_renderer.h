#pragma once

#include "framework64/renderer.h"

#include <libdragon.h>
#include <GL/gl.h>

struct fw64Renderer {
    uint32_t renderpass_count;
};

void fw64_libdragon_renderer_new_frame(fw64Renderer* renderer);

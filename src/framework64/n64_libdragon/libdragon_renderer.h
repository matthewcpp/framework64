#pragma once

#include "framework64/renderer.h"

#include <libdragon.h>
#include <GL/gl.h>

struct fw64Renderer {
    GLfloat clear_color[4];
};

void fw64_libdragon_renderer_init(fw64Renderer* renderer);

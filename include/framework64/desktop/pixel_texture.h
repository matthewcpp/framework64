#pragma once

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <vector>
#include <array>

#include "framework64/desktop/image.h"
#include "framework64/desktop/material.h"
#include "framework64/desktop/texture.h"

namespace framework64 {

struct SpriteVertex {
    float x, y, z;
    float u, v;
};

class PixelTexture {
public:
    void init();
    void setSize(int framebuffer_width, int framebuffer_height);

    std::array<SpriteVertex, 6> sprite_vertices;
    fw64Image image;
    fw64Texture texture;
    fw64Material material;
};

}
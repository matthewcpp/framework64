#pragma once

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <vector>
#include <array>

namespace framework64 {

struct SpriteVertex {
    float x, y, z;
    float u, v;
};

class PixelTexture {
public:
    void init();
    void setSize(int framebuffer_width, int framebuffer_height);
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void clear();
  
    GLuint gl_texture_handle;
    std::vector<uint8_t> pixel_data;
    int width, height;
    std::array<SpriteVertex, 6> sprite_vertices;
    std::array<float, 16> matrix;
};

}
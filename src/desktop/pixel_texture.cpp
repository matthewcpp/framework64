#include "framework64/desktop/pixel_texture.h"

#include "framework64/matrix.h"

#include <algorithm>

namespace framework64 {
void PixelTexture::init() {
    glGenTextures(1, &gl_texture_handle);
    glBindTexture(GL_TEXTURE_2D, gl_texture_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    sprite_vertices[0] = { -1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
    sprite_vertices[1] = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
    sprite_vertices[2] = { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f };

    sprite_vertices[3] = { -1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
    sprite_vertices[4] = { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    sprite_vertices[5] = { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f };

    matrix_set_identity(matrix.data());
}

void PixelTexture::setSize(int framebuffer_width, int framebuffer_height) {
    width = framebuffer_width;
    height = framebuffer_height;
    pixel_data.resize(framebuffer_width * framebuffer_height * 4);
}

void PixelTexture::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    int index = (y * width + x) * 4;
    pixel_data[index] = r;
    pixel_data[index + 1] = g;
    pixel_data[index + 2] = b;
    pixel_data[index + 3] = 255U;
}

void PixelTexture::clear() {
    std::fill(pixel_data.begin(), pixel_data.end(), 0);
}

}
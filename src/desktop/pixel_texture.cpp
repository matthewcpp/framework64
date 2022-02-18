#include "framework64/desktop/pixel_texture.h"

#include "framework64/matrix.h"

#include <algorithm>

namespace framework64 {
void PixelTexture::init() {
    sprite_vertices[0] = { -1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
    sprite_vertices[1] = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
    sprite_vertices[2] = { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f };

    sprite_vertices[3] = { -1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
    sprite_vertices[4] = { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    sprite_vertices[5] = { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f };

    texture.image = &image;
    material.texture = &texture;
    material.texture_frame = FW64_DESKTOP_ENTIRE_TEXTURE_FRAME;
}

void PixelTexture::setSize(int framebuffer_width, int framebuffer_height) {
    image.setSize(framebuffer_width, framebuffer_height, 1, 1);
}

}
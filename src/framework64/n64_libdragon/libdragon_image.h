#pragma once

#include <libdragon.h>
#include <GL/gl.h>

#include "framework64/image.h"

struct fw64Image {
    sprite_t* sprite;
    GLuint gl_handle;
};

void fw64_libdragon_image_init(fw64Image* image, sprite_t* sprite);
void fw64_libdragon_image_uninit(fw64Image* image, fw64Allocator* allocator);

#pragma once

#include "framework64/texture.h"
#include "framework64/n64_libdragon/image.h"

#include <GL/gl.h>

typedef struct {
    fw64Image* image;
} fw64TextureInfo;

struct fw64Texture {
    fw64TextureInfo info;
    GLuint texture_handle;
};
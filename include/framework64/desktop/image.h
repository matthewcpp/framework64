#pragma once

#include "framework64/image.h"

#include <gl/glew.h>

#include <string>

struct fw64Image {
    int width;
    int height;
    int hslices;
    int vslices;
    int ref_count = 0;
    GLuint gl_handle = 0;

    void freeGlResources();

    /** Load image file into OpenGL */
    static fw64Image* loadImageFile(std::string const& path);
    static fw64Image* loadImageBuffer(void* data, size_t size);
};
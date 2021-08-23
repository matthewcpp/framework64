#pragma once

#include <string>
#include <cstdint>

struct fw64Texture {
    int width;
    int height;
    int hslices;
    int vslices;
    uint32_t gl_handle;

    /** Load image file into OpenGL */
    static fw64Texture* loadImageFile(std::string const& path);

    /** Load image file and sprite metadata */
    static fw64Texture* loadTexture(std::string const& path);
};

#pragma once

#include "framework64/asset_database.h"
#include "framework64/image.h"

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <string>

struct fw64Image {
    int width;
    int height;
    int hslices;
    int vslices;
    GLuint gl_handle = 0;

    ~fw64Image();

public:
    static fw64Image* loadFromDatabase(fw64AssetDatabase* database, uint32_t index);

    /** Load image file into OpenGL */
    static fw64Image* loadImageFile(std::string const& path);
    static fw64Image* loadImageBuffer(void* data, size_t size);
};
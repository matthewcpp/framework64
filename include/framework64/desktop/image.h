#pragma once

#include "framework64/asset_database.h"
#include "framework64/image.h"

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <string>
#include <vector>

struct fw64Image {
    fw64Image() = default;
    ~fw64Image();

    void setSize(int w, int h, int h_slices, int v_slices);
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void updateGlImage();
    void clear();

    int width;
    int height;
    int hslices;
    int vslices;
    GLuint gl_handle = 0;

    std::vector<uint8_t> pixel_data;
    std::vector<GLuint> palettes;


public:
    static fw64Image* loadFromDatabase(fw64AssetDatabase* database, uint32_t index);

    /** Load image file into OpenGL */
    static fw64Image* loadImageFile(std::string const& path, bool isIndexMode);
    static bool addImagePaletteFromFile(fw64Image* image, std::string const& path);
    static fw64Image* loadImageBuffer(void* data, size_t size, bool isIndexMode);
};
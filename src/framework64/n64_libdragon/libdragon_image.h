#pragma once

#include <libdragon.h>
#include <GL/gl.h>

#include "framework64/image.h"

/** This needs to be kpt in touch with pipeline/n64_libdragon/processImage.js */
typedef struct {
    uint32_t hslices;
    uint32_t vslices;
} fw64LibdragonImageInfo;


struct fw64Image {
    fw64LibdragonImageInfo info;
    sprite_t* sprite;
    GLuint gl_handle;
};

void fw64_libdragon_image_init(fw64Image* image, sprite_t* sprite);
void fw64_libdragon_image_uninit(fw64Image* image, fw64Allocator* allocator);

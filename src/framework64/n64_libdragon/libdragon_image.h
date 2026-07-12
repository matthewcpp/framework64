#pragma once

#include <libdragon.h>
#include <GL/gl.h>

#include "framework64/image.h"

typedef struct {
    uint16_t hslices;
    uint16_t vslices;
} fw64LibdragonImageInfo;

typedef struct {
    /** libdragon specific sprite implementation */
    sprite_t* sprite;

    /** holds the raw uncompressed image data that is decoded by libdragon.
     *  This needs to be freed when the image is no longer needed. */
    char* mem_buffer;
    GLuint gl_handle;
} fw64LibdragonSpriteSlice;

struct fw64Image {
    fw64LibdragonImageInfo info;
    fw64LibdragonSpriteSlice* sprites;
};

int fw64_libdragon_image_init_from_datasource(fw64Image* image, fw64DataSource* datasource, fw64Allocator* allocator);
void fw64_libdragon_image_uninit(fw64Image* image, fw64Allocator* allocator);

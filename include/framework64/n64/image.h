#pragma once

#include "framework64/image.h"

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t hslices;
    uint16_t vslices;
} fw64N64ImageInfo;

struct fw64Image {
    fw64N64ImageInfo info;
    uint8_t* data;
};

int fw64_n64_image_init_from_rom(fw64Image* image, uint32_t assetIndex);
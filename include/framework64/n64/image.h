#pragma once

#include "framework64/allocator.h"
#include "framework64/image.h"

#define FW64_N64_IMAGE_FORMAT_RGBA16 0
#define FW64_N64_IMAGE_FORMAT_RGBA32 1

typedef struct {
    uint16_t format;
    uint16_t bpp;
    uint16_t width;
    uint16_t height;
    uint16_t hslices;
    uint16_t vslices;
} fw64N64ImageInfo;

struct fw64Image {
    fw64N64ImageInfo info;
    uint32_t rom_addr; // note this points to the start of the image data, i.e. base rom addr + sizeof(Info)
    uint8_t* data;
};

int fw64_n64_image_init_from_rom(fw64Image* image, uint32_t assetIndex, uint32_t options, fw64Allocator* allocator);

uint8_t* fw64_n64_image_get_data(fw64Image* image, int frame);
void fw64_n64_image_load_frame(fw64Image* image, int frame);
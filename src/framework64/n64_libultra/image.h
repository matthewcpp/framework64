#pragma once

#include "framework64/allocator.h"
#include "framework64/image.h"

#define FW64_N64_IMAGE_FORMAT_RGBA16 0
#define FW64_N64_IMAGE_FORMAT_RGBA32 1
#define FW64_N64_IMAGE_FORMAT_IA8 2
#define FW64_N64_IMAGE_FORMAT_IA4 3
#define FW64_N64_IMAGE_FORMAT_I8 4
#define FW64_N64_IMAGE_FORMAT_I4 5
#define FW64_N64_IMAGE_FORMAT_CI8 6
#define FW64_N64_IMAGE_FORMAT_CI4 7

// this needs to line up with createImageHeaderBuffer in pipeline/n64_libultra/ImageWriter.js
typedef struct {
    uint16_t format;
    uint16_t has_alpha;
    uint16_t width;
    uint16_t height;
    uint16_t hslices;
    uint16_t vslices;
    uint16_t palette_count;
    uint16_t palette_size;
} fw64N64ImageInfo;

struct fw64Image {
    fw64N64ImageInfo info;
    uint32_t rom_addr; // note this points to the start of the image data, i.e. base rom addr + sizeof(Info)
    uint8_t* data;
    uint16_t** palettes;
};

/** This is the general purpose method for reading non-dma mode images from a generic data source.*/
int fw64_n64_image_read_data(fw64Image* image, fw64DataSource* data_source, fw64Allocator* allocator);

/** 
 * This is the general purpose method for reading dma mode images from a filesystem handle.
 * Note: DMA mode is currently only supported for non-indexed images
 * */
int fw64_n64_image_init_dma_mode(fw64Image* image, int handle, uint32_t rom_address, fw64Allocator* allocator);

uint32_t fw64_n64_image_get_data_size(fw64Image* image);
uint32_t fw64_n64_image_get_frame_size(fw64Image* image);

uint8_t* fw64_n64_image_get_data(fw64Image* image, int frame);
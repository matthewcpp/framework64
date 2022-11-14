#include "framework64/n64/image.h"

#include "framework64/n64/asset_database.h"
#include "framework64/n64/filesystem.h"

#include <stdlib.h>
#include <stddef.h>

fw64Image* fw64_image_load(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    return fw64_image_load_with_options(asset_database, asset_index, FW64_IMAGE_FLAG_NONE, allocator);
}

fw64Image* fw64_image_load_with_options(fw64AssetDatabase* asset_database, uint32_t asset_index, uint32_t options, fw64Allocator* allocator) {
    (void)asset_database;
    if (!allocator) allocator = fw64_default_allocator();
    
    fw64Image img;
    if (fw64_n64_image_init_from_rom(&img, asset_index, options, allocator)) {
        fw64Image* image = allocator->malloc(allocator, sizeof(fw64Image));
        *image = img;

        return image;
    }
    else {
        return NULL;
    }
}

#define IMAGE_IS_DMA_MODE(image) ((image)->rom_addr != 0)

uint32_t fw64_n64_image_get_data_size(fw64Image* image) {
    switch (image->info.format) {
    case FW64_N64_IMAGE_FORMAT_RGBA16:
        return image->info.width * image->info.height * 2;

    case FW64_N64_IMAGE_FORMAT_RGBA32:
        return image->info.width * image->info.height * 4;

    case FW64_N64_IMAGE_FORMAT_IA8:
    case FW64_N64_IMAGE_FORMAT_I8:
    case FW64_N64_IMAGE_FORMAT_CI8:
        return image->info.width * image->info.height;

    case FW64_N64_IMAGE_FORMAT_IA4:
    case FW64_N64_IMAGE_FORMAT_I4:
    case FW64_N64_IMAGE_FORMAT_CI4:
        return (image->info.width * image->info.height) / 2;
    }
}

uint32_t fw64_n64_image_get_frame_size(fw64Image* image) {
    return fw64_n64_image_get_data_size(image) / (image->info.hslices * image->info.vslices);
}

int fw64_n64_image_init_from_rom(fw64Image* image, uint32_t asset_index, uint32_t options, fw64Allocator* allocator) {
    int handle = fw64_filesystem_open(asset_index);
    if (handle < 0)
        return 0;

    int bytes_read = fw64_filesystem_read(&image->info, sizeof(fw64N64ImageInfo), 1, handle);

    if (bytes_read != sizeof(fw64N64ImageInfo)) {
        fw64_filesystem_close(handle);
        return 0;
    }

    if (options & FW64_IMAGE_FLAG_DMA_MODE) {
        uint32_t frame_size = fw64_n64_image_get_frame_size(image);
        image->rom_addr = fw64_n64_filesystem_get_rom_address(asset_index) + sizeof(fw64N64ImageInfo);
        image->data = allocator->memalign(allocator, 8, frame_size);
        fw64_image_load_frame(image, 0);

        // TODO: need to seek filesystem stream
        if (image->info.palette_count > 0) {
            uint32_t seek_amount = fw64_n64_image_get_data_size(image) - frame_size;

        }
    }
    else {
        int data_size = fw64_n64_image_get_data_size(image);
        uint8_t* image_data = allocator->memalign(allocator, 8, data_size);
        bytes_read = fw64_filesystem_read(image_data, data_size, 1, handle);

        if (bytes_read != data_size) {
            allocator->free(allocator, image_data);
            return 0;
        }

        image->data = image_data;
        image->rom_addr = 0;
    }

    if (image->info.palette_count > 0) {
        image->palettes = allocator->malloc(allocator, sizeof(uint16_t*) * image->info.palette_count);
        uint32_t palette_data_size = image->info.palette_size * sizeof(uint16_t);

        for (uint16_t i = 0; i < image->info.palette_count; i++) {
            uint16_t* color_data = allocator->memalign(allocator, 8, palette_data_size);
            fw64_filesystem_read(color_data, palette_data_size, 1, handle);

            image->palettes[i] = color_data;
        }
    }
    else {
        image->palettes = NULL;
    }

    fw64_filesystem_close(handle);
}

void fw64_image_delete(fw64AssetDatabase* asset_database, fw64Image* image, fw64Allocator* allocator) {
    if (!allocator) allocator = fw64_default_allocator();
    allocator->free(allocator, image->data);

    if (image->palettes) {
        for (uint16_t i = 0; i < image->info.palette_count; i++) {
            allocator->free(allocator, image->palettes[i]);
        }

        allocator->free(allocator, image->palettes);
    }

    allocator->free(allocator, image);
}

uint8_t* fw64_n64_image_get_data(fw64Image* image, int frame) {
    if (IMAGE_IS_DMA_MODE(image))
        return image->data;
    else
        return image->data + (fw64_n64_image_get_frame_size(image) * frame);
    
}

void fw64_image_load_frame(fw64Image* image, uint32_t frame) {
    if (!IMAGE_IS_DMA_MODE(image))
        return;

    uint32_t frame_size = fw64_n64_image_get_frame_size(image);
    uint32_t rom_location = image->rom_addr + (frame_size * frame);
    fw64_n64_filesystem_raw_read(image->data, rom_location, frame_size);
}

uint16_t fw64_image_get_palette_count(fw64Image* image) {
    return image->info.palette_count;
}

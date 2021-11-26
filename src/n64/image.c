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

static inline uint32_t fw64_n64_image_get_frame_size(fw64Image* image) {
    return (image->info.width / image->info.hslices) * (image->info.height / image->info.vslices) * image->info.bpp;
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
        image->rom_addr = fw64_n64_filesystem_get_rom_address(asset_index) + sizeof(fw64N64ImageInfo);
        image->data = allocator->memalign(allocator, 8, fw64_n64_image_get_frame_size(image));
        fw64_n64_image_load_frame(image, 0);
    }
    else {
        int data_size = image->info.width * image->info.height * image->info.bpp;
        uint8_t* image_data = allocator->memalign(allocator, 8, data_size);
        bytes_read = fw64_filesystem_read(image_data, data_size, 1, handle);

        if (bytes_read != data_size) {
            allocator->free(allocator, image_data);
            return 0;
        }

        image->data = image_data;
        image->rom_addr = 0;
    }

    fw64_filesystem_close(handle);
}

void fw64_image_delete(fw64AssetDatabase* asset_database, fw64Image* image, fw64Allocator* allocator) {
    if (!allocator) allocator = fw64_default_allocator();
    allocator->free(allocator, image->data);
    allocator->free(allocator, image);
}

uint8_t* fw64_n64_image_get_data(fw64Image* image, int frame) {
    if (IMAGE_IS_DMA_MODE(image))
        return image->data;
    else
        return image->data + (fw64_n64_image_get_frame_size(image) * frame);
    
}

void fw64_n64_image_load_frame(fw64Image* image, int frame) {
    if (!IMAGE_IS_DMA_MODE(image))
        return;

    uint32_t frame_size = fw64_n64_image_get_frame_size(image);
    uint32_t rom_location = image->rom_addr + (frame_size * frame);
    fw64_n64_filesystem_raw_read(image->data, rom_location, frame_size);
}
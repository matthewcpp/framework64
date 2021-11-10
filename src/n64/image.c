#include "framework64/n64/image.h"

#include "framework64/n64/asset_database.h"
#include "framework64/filesystem.h"

#include <stdlib.h>
#include <stddef.h>

fw64Image* fw64_image_load(fw64AssetDatabase* database, uint32_t asset_index, fw64Allocator* allocator) {
    (void)database;
    if (!allocator) allocator = fw64_default_allocator();
    
    fw64Image img;
    if (fw64_n64_image_init_from_rom(&img, asset_index, allocator)) {
        fw64Image* image = allocator->malloc(allocator, sizeof(fw64Image));
        *image = img;

        return image;
    }
    else {
        return NULL;
    }
}

int fw64_n64_image_init_from_rom(fw64Image* image, uint32_t asset_index, fw64Allocator* allocator) {
    int handle = fw64_filesystem_open(asset_index);
    if (handle < 0)
        return 0;

    fw64N64ImageInfo image_info;

    int bytes_read = fw64_filesystem_read(&image_info, sizeof(fw64N64ImageInfo), 1, handle);

    if (bytes_read != sizeof(fw64N64ImageInfo)) {
        fw64_filesystem_close(handle);
        return 0;
    }

    int data_size = image_info.width * image_info.height * image_info.bpp;
    uint8_t* image_data = allocator->memalign(allocator, 8, data_size);
    bytes_read = fw64_filesystem_read(image_data, data_size, 1, handle);
    fw64_filesystem_close(handle);

    if (bytes_read != data_size) {
        allocator->free(allocator, image_data);
        return 0;
    }

    image->info = image_info;
    image->data = image_data;
}

void fw64_image_delete(fw64AssetDatabase* asset_database, fw64Image* image, fw64Allocator* allocator) {
    if (!allocator) allocator = fw64_default_allocator();
    allocator->free(allocator, image->data);
    allocator->free(allocator, image);
}

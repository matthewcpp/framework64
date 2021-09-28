#include "framework64/n64/image.h"
#include "framework64/filesystem.h"


#include <stdlib.h>
#include <malloc.h>
#include <stddef.h>

fw64Image* fw64_image_load(fw64AssetDatabase* database, uint32_t assetIndex) {
    (void)database;
    
    fw64Image img;
    
    if (fw64_n64_image_init_from_rom(&img, assetIndex)) {
        fw64Image* image = malloc(sizeof(fw64Image));
        *image = img;

        return image;
    }
    else {
        return NULL;
    }
}

int fw64_n64_image_init_from_rom(fw64Image* image, uint32_t assetIndex) {
    int handle = fw64_filesystem_open(assetIndex);
    if (handle < 0)
        return 0;

    fw64N64ImageInfo image_info;

    int bytes_read = fw64_filesystem_read(&image_info, sizeof(fw64N64ImageInfo), 1, handle);

    if (bytes_read != sizeof(fw64N64ImageInfo)) {
        fw64_filesystem_close(handle);
        return 0;
    }

    int data_size = image_info.width * image_info.height * 2;
    uint8_t* image_data = memalign(8, data_size);
    bytes_read = fw64_filesystem_read(image_data, data_size, 1, handle);
    fw64_filesystem_close(handle);

    if (bytes_read != data_size) {
        free(image_data);
        return 0;
    }

    image->info = image_info;
    image->data = image_data;
}

void fw64_image_delete(fw64Image* image) {
    free(image->data);
    free(image);
}
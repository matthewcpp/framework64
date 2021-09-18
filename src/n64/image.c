#include "framework64/n64/image.h"
#include "framework64/filesystem.h"


#include <stdlib.h>
#include <malloc.h>
#include <stddef.h>

fw64Image* fw64_n64_image_load(int assetIndex) {
    int handle = fw64_filesystem_open(assetIndex);
    if (handle < 0)
        return NULL;

    fw64N64ImageInfo image_info;

    int bytes_read = fw64_filesystem_read(&image_info, sizeof(fw64N64ImageInfo), 1, handle);

    if (bytes_read != sizeof(fw64N64ImageInfo)) {
        fw64_filesystem_close(handle);
        return NULL;
    }

    int data_size = image_info.width * image_info.height * 2;
    uint8_t* image_data = memalign(8, data_size);
    bytes_read = fw64_filesystem_read(image_data, data_size, 1, handle);
    fw64_filesystem_close(handle);

    if (bytes_read != data_size) {
        free(image_data);
        return NULL;
    }

    fw64Image* image = malloc(sizeof(fw64Image));
    image->info = image_info;
    image->data = image_data;
    image->ref_count = 0;

    return image;
}

void fw64_n64_image_delete(fw64Image* image) {
    free(image->data);
    free(image);
}

void fw64_image_reference_add(fw64Image* image) {
    image->ref_count += 1;
}

void fw64_image_reference_remove(fw64Image* image) {
    image->ref_count -= 1;

    if (image->ref_count <= 0) {
        fw64_n64_image_delete(image);
    }
}
#include "framework64/n64_libdragon/image.h"

typedef struct {
    uint32_t size;
} fw64LibdragonImageHeader;

fw64Image* fw64_image_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator) {
    fw64Image* image = allocator->malloc(allocator,sizeof(fw64Image));

    int result = fw64_libdragon_init_image_from_datasource(image, data_source, allocator);

    if (!result) {
        allocator->free(allocator, image);
        return NULL;
    }
    else {
        return image;
    }
}

int fw64_libdragon_init_image_from_datasource(fw64Image* image, fw64DataSource* data_source, fw64Allocator* allocator) {
    fw64LibdragonImageHeader header;
    fw64_data_source_read(data_source, &header, sizeof(fw64LibdragonImageHeader), 1);

    debugf("fw64_libdragon_init_image_from_datasource: %d\n", (int)header.size);

    image->buffer = allocator->malloc(allocator,  header.size);

    fw64_data_source_read(data_source, image->buffer, 1, header.size);
    image->libdragon_sprite = sprite_load_buf(image->buffer, header.size);

    return 1;
}

void fw64_libdragon_image_uninit(fw64Image* image, fw64Allocator* allocator) {
    sprite_free(image->libdragon_sprite);
    allocator->free(allocator, image->buffer);
}

void fw64_image_delete(fw64AssetDatabase* asset_database, fw64Image* image, fw64Allocator* allocator) {
    fw64_libdragon_image_uninit(image, allocator);
    allocator->free(allocator, image);
}
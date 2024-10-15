#include "libdragon_image.h"

void fw64_image_delete(fw64AssetDatabase* asset_database, fw64Image* image, fw64Allocator* allocator) {
    (void)asset_database;

    fw64_allocator_free(allocator, image->libdragon_sprite);
    fw64_allocator_free(allocator, image);
}

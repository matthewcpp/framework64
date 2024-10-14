#include "libdragon_asset_database.h"

#include "libdragon_image.h"

#include <libdragon.h>

fw64Image* fw64_assets_load_image(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    (void)asset_database;

    fw64Image* image = fw64_allocator_malloc(allocator, sizeof(fw64Image));
    image->libdragon_sprite = sprite_load(asset_id);
    return image;
}
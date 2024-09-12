#include "framework64/util/texture_util.h"

fw64Texture* fw64_texture_util_create_from_loaded_image(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64Image* image = fw64_assets_load_image(asset_database, asset_id, allocator);

    if (!image) {
        return NULL;
    }

    return fw64_texture_create_from_image(image, allocator);
}

void fw64_texture_util_delete_tex_and_image(fw64Texture* texture, fw64AssetDatabase* asset_database, fw64Allocator* allocator) {
    fw64Image* image = fw64_texture_get_image(texture);

    if (image) {
        fw64_image_delete(asset_database, image, allocator);
    }

    fw64_texture_delete(texture, allocator);
}

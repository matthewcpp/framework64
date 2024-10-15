#include "libdragon_material_bundle.h"

#include <libdragon.h>

#include <string.h>

fw64MaterialBundle* fw64_material_bundle_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    sprite_t* sprite = sprite_load("rom:/0");
    if (!sprite) {
        return NULL;
    }

    fw64MaterialBundle* material_bundle = fw64_allocator_malloc(allocator, sizeof(fw64MaterialBundle));

    // read material bundle info
    material_bundle->material_count = 1;
    material_bundle->texture_count = 1;
    material_bundle->image_count = 1;

    // allocate and read bundle info
    material_bundle->materials = fw64_allocator_malloc(allocator, sizeof(fw64Material) * material_bundle->material_count);
    material_bundle->textures = fw64_allocator_malloc(allocator, sizeof(fw64Texture) * material_bundle->texture_count);
    material_bundle->images = fw64_allocator_malloc(allocator, sizeof(fw64Image) * material_bundle->image_count);

    // temp hardcoded implementation
    fw64_libdragon_image_init(material_bundle->images, sprite);
    material_bundle->textures->image = material_bundle->images;

    material_bundle->materials->texture = material_bundle->textures;
    material_bundle->materials->shading_mode = FW64_SHADING_MODE_UNLIT_TEXTURED;

    return material_bundle;
}

void fw64_material_bundle_delete(fw64MaterialBundle* material_bundle, fw64Allocator* allocator) {
    // TODO: need to free all the stuff
    fw64_allocator_free(allocator, material_bundle);
}
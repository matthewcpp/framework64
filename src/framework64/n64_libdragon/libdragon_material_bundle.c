#include "libdragon_material_bundle.h"

#include "libdragon_asset_database.h"
#include "libdragon_material.h"

#include <libdragon.h>

#include <string.h>

fw64MaterialBundle* fw64_material_bundle_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    fw64MaterialBundle* material_bundle = allocator->malloc(allocator, sizeof(fw64MaterialBundle));

    fw64_data_source_read(data_source, &material_bundle->info, sizeof(fw64MaterialBundleInfo), 1);

    if (material_bundle->info.image_count > 0) {
        material_bundle->images = allocator->malloc(allocator, sizeof(fw64Image) * material_bundle->info.image_count);

        uint32_t image_id;
        for (uint32_t i = 0; i < material_bundle->info.image_count; i++) {
            fw64_data_source_read(data_source, &image_id, sizeof(uint32_t), 1);
            sprite_t* sprite = fw64_libdragon_assets_load_dfs_sprite(assets, image_id, allocator);
            fw64_libdragon_image_init(material_bundle->images + i, sprite);
        }
    }
    else {
        material_bundle->images = NULL;
    }

    if (material_bundle->info.texture_count > 0) {
        material_bundle->textures = allocator->malloc(allocator, sizeof(fw64Texture) * material_bundle->info.texture_count);
        
        for (uint32_t i = 0; i < material_bundle->info.texture_count; i++) {
            fw64Texture* texture = material_bundle->textures + i;
            fw64_data_source_read(data_source, &texture->image, sizeof(uint32_t), 1);
            
            // fixup the image pointer
            uint32_t image_index = (uint32_t)texture->image;
            texture->image = material_bundle->images + image_index;
        }
    }
    else {
        material_bundle->textures = NULL;
    }

    if (material_bundle->info.material_count > 0) {
        material_bundle->materials = allocator->malloc(allocator, sizeof(fw64Material) * material_bundle->info.material_count);
        fw64_data_source_read(data_source, material_bundle->materials, sizeof(fw64Material), material_bundle->info.material_count);
    }
    else {
        material_bundle->materials = NULL;
    }

    // fixup texture pointers for materials
    for (uint32_t i = 0; i < material_bundle->info.material_count; i++) {
        fw64Material* material = material_bundle->materials + i;

        uint32_t material_texture_index = (uint32_t)material->texture;
        if (material_texture_index == FW64_MATERIAL_NO_TEXTURE) {
            material->texture = NULL;
        } else {
            material->texture = material_bundle->textures + material_texture_index;
        }
    }

    return material_bundle;
}

void fw64_material_bundle_delete(fw64MaterialBundle* material_bundle, fw64Allocator* allocator) {
    for (uint32_t i = 0; i < material_bundle->info.image_count; i++) {
        fw64_libdragon_image_uninit(material_bundle->images + i, allocator);
    }

    allocator->free(allocator, material_bundle->images);
    allocator->free(allocator, material_bundle->textures);
    allocator->free(allocator, material_bundle->materials);
    allocator->free(allocator, material_bundle);
}

#if 0
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
#endif
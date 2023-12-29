#include "framework64/n64_libdragon/material_bundle.h"

fw64MaterialBundle* fw64_material_bundle_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    fw64MaterialBundle* material_bundle = allocator->malloc(allocator, sizeof(fw64MaterialBundle));

    fw64_data_source_read(data_source, &material_bundle->info, sizeof(fw64MaterialBundleInfo), 1);

    if (material_bundle->info.image_count > 0) {
        debugf("Read %u Images\n", material_bundle->info.image_count);
        material_bundle->images = allocator->malloc(allocator, sizeof(fw64Image) * material_bundle->info.image_count);

        for (uint32_t i = 0; i < material_bundle->info.image_count; i++) {
            fw64_libdragon_init_image_from_datasource(material_bundle->images + i, data_source, allocator);
        }
    }
    else {
        material_bundle->images = NULL;
    }

    if (material_bundle->info.texture_count > 0) {
        debugf("Read %u Textures\n", material_bundle->info.texture_count);
        material_bundle->textures = allocator->malloc(allocator, sizeof(fw64Texture) * material_bundle->info.texture_count);
        
        for (uint32_t i = 0; i < material_bundle->info.material_count; i++) {
            fw64Texture* texture = material_bundle->textures + i;
            fw64_data_source_read(data_source, &texture->info, sizeof(fw64TextureInfo), 1);
        }
    }
    else {
        material_bundle->textures = NULL;
    }

    if (material_bundle->info.material_count > 0) {
        debugf("Read %u Materials\n", material_bundle->info.material_count);
        material_bundle->materials = allocator->malloc(allocator, sizeof(fw64Material) * material_bundle->info.material_count);
        fw64_data_source_read(data_source, material_bundle->materials, sizeof(fw64Material), material_bundle->info.material_count);
    }
    else {
        material_bundle->materials = NULL;
    }

    // fixup image pointers and create GL Textures
    for (uint32_t i = 0; i < material_bundle->info.texture_count; i++) {
        fw64Texture* texture = material_bundle->textures + i;

        uint32_t image_index = (uint32_t)texture->info.image;
        texture->info.image = material_bundle->images + image_index;

        glGenTextures(1, &texture->texture_handle);
        glBindTexture(GL_TEXTURE_2D, texture->texture_handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        if (texture->info.image->libdragon_sprite == NULL) {
            debugf("fw64_material_bundle_load_from_datasource: sprite is NULL\n");
        }
        //glSpriteTextureN64(GL_TEXTURE_2D, texture->info.image->libdragon_sprite, &(rdpq_texparms_t){.s.repeats = REPEAT_INFINITE, .t.repeats = REPEAT_INFINITE});
    }

    // fixup texture pointers for materials
    for (uint32_t i = 0; i < material_bundle->info.material_count; i++) {
        fw64Material* material = material_bundle->materials + i;

        uint32_t material_texture_index = (uint32_t)material->texture;
        if (material_texture_index == FW64_MATERIAL_NO_TEXTURE)
            material->texture = NULL;
        else
            material->texture = material_bundle->textures + material_texture_index;
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
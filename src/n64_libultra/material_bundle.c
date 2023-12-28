#include <framework64/n64/material_bundle.h>

#include <framework64/n64/mesh.h>
#include <framework64/material.h>

#define FW64_MESH_RESOURCES_HEADER_SIZE 3 * sizeof(uint32_t)

fw64MaterialBundle* fw64_material_bundle_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase*, fw64Allocator* allocator) {
    fw64MaterialBundle* resources = allocator->malloc(allocator, sizeof(fw64MaterialBundle));
    fw64_data_source_read(data_source, resources, FW64_MESH_RESOURCES_HEADER_SIZE, 1);
    resources->flags = 0;

    if (resources->image_count > 0) {
        resources->images = allocator->malloc(allocator, sizeof(fw64Image) * resources->image_count);

        for (uint32_t i = 0; i < resources->image_count; i++) {
            fw64_n64_image_read_data(resources->images + i, data_source, allocator);
        }
    }
    else {
        resources->images = NULL;
    }

    if (resources->texture_count > 0) {
        resources->textures = allocator->malloc(allocator, resources->texture_count * sizeof(fw64Texture));
        fw64_data_source_read(data_source, resources->textures , sizeof(fw64Texture), resources->texture_count);
    }
    else {
        resources->textures = NULL;
    }

    if (resources->material_count > 0) { // this should always be the case
        resources->materials = allocator->malloc(allocator, sizeof(fw64Material) * resources->material_count);
        fw64_data_source_read(data_source, resources->materials, sizeof(fw64Material), resources->material_count);
    }
    else {
        resources->materials = NULL;
    }

    // fixup image pointers for textures
    for (uint32_t i = 0; i < resources->texture_count; i++) {
        fw64Texture* texture = resources->textures + i;

        uint32_t image_index = (uint32_t)texture->image;
        texture->image = resources->images + image_index;
    }

    // fixup texture pointers for materials
    for (uint32_t i = 0; i < resources->material_count; i++) {
        fw64Material* material = resources->materials + i;

        uint32_t material_texture_index = (uint32_t)material->texture;
        if (material_texture_index == FW64_MATERIAL_NO_TEXTURE)
            material->texture = NULL;
        else
            material->texture = resources->textures + material_texture_index;
    }

    return resources;
}

void fw64_material_bundle_delete(fw64MaterialBundle* material_bundle, fw64Allocator* allocator) {
    if (material_bundle->material_count > 0) {
        allocator->free(allocator, material_bundle->materials);
    }

    if (material_bundle->texture_count > 0) {
        allocator->free(allocator, material_bundle->textures);
    }

    int delete_images = !(material_bundle->flags & FW64_MESH_FLAGS_IMAGES_ARE_SHARED);

    if (delete_images && material_bundle->image_count > 0) {
        for (uint32_t i = 0; i < material_bundle->image_count; i++) {
            fw64Image* image = material_bundle->images + i;
            allocator->free(allocator, image->data);
        }
        
        allocator->free(allocator, material_bundle->images);
    }

    allocator->free(allocator, material_bundle);
}

void fw64_n64_material_bundle_init(fw64MaterialBundle* material_bundle, uint32_t image_count, uint32_t texture_count, uint32_t material_count, fw64Allocator* allocator) {
    material_bundle->image_count = image_count;
    if (image_count > 0) {
        material_bundle->images = allocator->malloc(allocator, image_count * sizeof(fw64Image));
    } else {
        material_bundle->images = NULL;
    }

    material_bundle->texture_count = texture_count;
    if (texture_count > 0) {
        material_bundle->textures = allocator->malloc(allocator, texture_count * sizeof(fw64Texture));
    } else {
        material_bundle->textures = NULL;
    }

    material_bundle->material_count = material_count;
    if (material_count > 0) {
        material_bundle->materials = allocator->malloc(allocator, material_count * sizeof(fw64Material));
    } else {
        material_bundle->materials = NULL;
    }
}

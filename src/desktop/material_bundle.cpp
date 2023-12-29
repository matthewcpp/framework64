#include "framework64/desktop/material_bundle.hpp"

#include "framework64/desktop/asset_database.hpp"

#include <cassert>
#include <limits>

struct MaterialBundleInfo {
    uint32_t image_count;
    uint32_t texture_count;
    uint32_t material_count;
};

/// Note this needs to correspond to Material.js NoTexture define
#define MATERIAL_BUNDLE_INVALID_INDEX std::numeric_limits<uint32_t>::max()

fw64MaterialBundle* fw64MaterialBundle::loadFromDatasource(fw64DataSource* data_source, framework64::ShaderCache& shader_cache, fw64Allocator* allocator) {
    MaterialBundleInfo material_bundle_info;
    auto material_bundle = std::make_unique<fw64MaterialBundle>();
    fw64_data_source_read(data_source, &material_bundle_info, sizeof(MaterialBundleInfo), 1);

    material_bundle->images.resize(material_bundle_info.image_count);
    for (uint32_t i = 0; i < material_bundle_info.image_count; i++) {
        auto* image = fw64Image::loadFromDatasource(data_source, allocator);
        material_bundle->images[i].reset(image);
    }

    for (uint32_t i = 0; i < material_bundle_info.texture_count; i++) {
        struct TextureST {uint32_t s,t;};
        auto& texture = material_bundle->textures.emplace_back(std::make_unique<fw64Texture>());

        uint32_t image_index;
        TextureST texture_st;

        fw64_data_source_read(data_source, &image_index, sizeof(uint32_t), 1);
        fw64_data_source_read(data_source, &texture_st, sizeof(TextureST), 1);

        texture->wrap_s = static_cast<fw64TextureWrapMode>(texture_st.s);
        texture->wrap_t = static_cast<fw64TextureWrapMode>(texture_st.t);

        if (image_index != MATERIAL_BUNDLE_INVALID_INDEX) {
            texture->image = material_bundle->images[image_index].get();
        }
    }

    for (uint32_t i = 0; i < material_bundle_info.material_count; i++) {
        auto& material = material_bundle->materials.emplace_back(std::make_unique<fw64Material>());

        uint32_t texture_index, shading_mode;
        fw64_data_source_read(data_source, &texture_index, sizeof(uint32_t), 1);
        fw64_data_source_read(data_source, &shading_mode, sizeof(uint32_t), 1);
        fw64_data_source_read(data_source, material->color.data(), sizeof(float), 4);

        material->shading_mode = static_cast<fw64ShadingMode>(shading_mode);

        if (texture_index != MATERIAL_BUNDLE_INVALID_INDEX) {
            material->texture = material_bundle->textures[texture_index].get();
        }
    }

    for (auto& material : material_bundle->materials) {
        material->shader = shader_cache.getShaderProgram(material->shading_mode);
    }
    
    return material_bundle.release();
}

// C interface

fw64MaterialBundle* fw64_material_bundle_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    assert(assets != nullptr);
    assert(allocator != nullptr);\

    return fw64MaterialBundle::loadFromDatasource(data_source, assets->shader_cache, allocator);
}

void fw64_material_bundle_delete(fw64MaterialBundle* material_bundle, fw64Allocator*) {
    assert(material_bundle != nullptr);

    delete(material_bundle);
}
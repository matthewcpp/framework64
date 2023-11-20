#include "framework64/desktop/asset_database.h"

#include "framework64/skinned_mesh.h"
#include "framework64/desktop/image.h"
#include "framework64/desktop/font.h"
#include "framework64/desktop/mesh.h"

#include "framework64/scene.h"

#include <iostream>

fw64Image* fw64_assets_load_image(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    framework64::FileDataSource file_datasource;
    if (!asset_database->openAssetFile(asset_index, file_datasource))
        return nullptr;


    return fw64_image_load_from_datasource(&file_datasource.interface, allocator);
}

fw64Image* fw64_assets_load_image_dma(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    return fw64_assets_load_image(asset_database, asset_index, allocator);
}

fw64Font* fw64_assets_load_font(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    framework64::FileDataSource file_datasource;
    if (!asset_database->openAssetFile(asset_index, file_datasource))
        return nullptr;

    return fw64Font::loadFromDatasource(&file_datasource.interface, allocator);
}

fw64Mesh* fw64_assets_load_mesh(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    framework64::FileDataSource file_datasource;
    if (!asset_database->openAssetFile(asset_index, file_datasource))
        return nullptr;

    return fw64Mesh::loadFromDatasource(&file_datasource.interface, nullptr, asset_database->shader_cache, allocator);
}

fw64Scene* fw64_assets_load_scene(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    framework64::FileDataSource file_datasource;
    if (!asset_database->openAssetFile(asset_index, file_datasource))
        return nullptr;

    return fw64_scene_load_from_datasource(&file_datasource.interface, asset_database, allocator);
}

fw64AnimationData* fw64_assets_load_animation_data(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    framework64::FileDataSource file_datasource;
    if (!asset_database->openAssetFile(asset_index, file_datasource))
        return nullptr;

    return fw64_animation_data_load_from_datasource(&file_datasource.interface, allocator);
}

fw64SkinnedMesh* fw64_assets_load_skinned_mesh(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    framework64::FileDataSource file_datasource;
    if (!asset_database->openAssetFile(asset_index, file_datasource))
        return nullptr;

    return fw64_skinned_mesh_load_from_datasource(&file_datasource.interface, asset_database, allocator);
}

std::filesystem::path fw64AssetDatabase::getAssetPath(fw64AssetId asset_id) const {
    auto result = asset_bundle.find(asset_id);

    if (result == asset_bundle.end()) {
        return {};
    }

    return asset_directory / result->second;
}

bool fw64AssetDatabase::openAssetFile(fw64AssetId asset_id, framework64::FileDataSource & datasource) const {
    auto asset_path = getAssetPath(asset_id);

    if (asset_path.empty()) {
        return false;
    }
    else {
        return datasource.open(asset_path);
    }
}

bool fw64AssetDatabase::init() {
    auto const bundle_path = asset_directory / "asset_bundle.txt";
    std::ifstream file(bundle_path);

    if (!file)
        return false;

    size_t count;
    file >> count;

    for (size_t i = 0; i < count; i++) {
        fw64AssetId id;
        std::string path;

        file >> id >> path;
        asset_bundle[id] = path;
    }

    return true;
}

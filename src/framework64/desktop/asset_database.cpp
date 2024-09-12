#include "framework64/desktop/asset_database.hpp"

#include "framework64/scene.h"
#include "framework64/skinned_mesh.h"

#include "framework64/desktop/image.hpp"
#include "framework64/desktop/font.hpp"
#include "framework64/desktop/mesh.hpp"

#include <iostream>

fw64Image* fw64_assets_load_image(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    auto* file_datasource = asset_database->openAssetFile(asset_id);

    if (!file_datasource)
        return nullptr;

    auto* image = fw64_image_load_from_datasource(&file_datasource->interface, allocator);
    file_datasource->close();

    return image;
}

fw64Image* fw64_assets_load_image_dma(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    return fw64_assets_load_image(asset_database, asset_id, allocator);
}

fw64Font* fw64_assets_load_font(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    auto* file_datasource = asset_database->openAssetFile(asset_id);

    if (!file_datasource)
        return nullptr;

    auto* font = fw64Font::loadFromDatasource(&file_datasource->interface, allocator);
    file_datasource->close();

    return font;
}

fw64Mesh* fw64_assets_load_mesh(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    auto* file_datasource = asset_database->openAssetFile(asset_id);

    if (!file_datasource)
        return nullptr;

    auto* mesh = fw64Mesh::loadFromDatasource(&file_datasource->interface, nullptr, asset_database->shader_cache, allocator);
    file_datasource->close();

    return mesh;
}

fw64Scene* fw64_assets_load_scene(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    auto* file_datasource = asset_database->openAssetFile(asset_id);

    if (!file_datasource)
        return nullptr;

    auto* scene = fw64_scene_load_from_datasource(&file_datasource->interface, asset_database, allocator);
    file_datasource->close();

    return scene;
}

fw64AnimationData* fw64_assets_load_animation_data(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    auto* file_datasource = asset_database->openAssetFile(asset_id);

    if (!file_datasource)
        return nullptr;

    auto* animation_data =  fw64_animation_data_load_from_datasource(&file_datasource->interface, allocator);
    file_datasource->close();

    return animation_data;
}

fw64SkinnedMesh* fw64_assets_load_skinned_mesh(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    auto* file_datasource = asset_database->openAssetFile(asset_id);

    if (!file_datasource)
        return nullptr;

    auto* skinned_mesh = fw64_skinned_mesh_load_from_datasource(&file_datasource->interface, asset_database, allocator);
    file_datasource->close();

    return skinned_mesh;
}

fw64DataSource* fw64_assets_open_datasource(fw64AssetDatabase* asset_database, fw64AssetId asset_id) {
    auto* datasource = asset_database->openAssetFile(asset_id);

    return datasource ? &datasource->interface : nullptr;
}

void fw64_assets_close_datasource(fw64AssetDatabase*, fw64DataSource* datasource) {
    auto* file_datasource = reinterpret_cast<framework64::FileDataSource*>(datasource);
    file_datasource->close();
}

std::filesystem::path fw64AssetDatabase::getAssetPath(fw64AssetId asset_id) const {
    auto result = asset_bundle.find(asset_id);

    if (result == asset_bundle.end()) {
        return {};
    }

    return asset_directory / result->second;
}

framework64::FileDataSource* fw64AssetDatabase::openAssetFile(fw64AssetId asset_id) {
    auto asset_path = getAssetPath(asset_id);

    if (asset_path.empty()) {
        return nullptr;
    }

    for (auto& file_datasource : data_sources) {
        if (file_datasource.file.is_open()) {
            continue;
        }

        return file_datasource.open(asset_path) ? &file_datasource : nullptr;
    }

    return nullptr;
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

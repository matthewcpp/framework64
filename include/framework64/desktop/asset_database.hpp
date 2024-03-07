#pragma once

#include "framework64/asset_database.h"
#include "framework64/desktop/file_datasource.hpp"
#include "framework64/desktop/shader_cache.hpp"

#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

class fw64AssetDatabase {
public:
    fw64AssetDatabase(std::string const & dir, framework64::ShaderCache& sc)
        : asset_dir(dir), asset_directory(dir), shader_cache(sc){}

    bool init();
    std::filesystem::path getAssetPath(fw64AssetId asset_id) const;
    framework64::FileDataSource* openAssetFile(fw64AssetId asset_id);

    std::string asset_dir;
    std::filesystem::path asset_directory;
    framework64::ShaderCache& shader_cache;

    std::array<framework64::FileDataSource, FW64_ASSETS_MAX_OPEN_DATASOURCES> data_sources;

    std::unordered_map<fw64AssetId, std::string> asset_bundle;
};


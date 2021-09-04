#pragma once

#include "framework64/assets.h"
#include "framework64/desktop/shader_cache.h"

#include <sqlite3.h>

#include <string>
#include <unordered_map>
#include <memory>

class fw64Assets {
public:
    fw64Assets(std::string asset_dir_path, framework64::ShaderCache& sc) : asset_dir(asset_dir_path), shader_cache(sc) {}
    bool init();

    fw64Texture* getTexture(int handle);
    fw64Font* getFont(int handle);
    fw64Mesh* getMesh(int handle);

    bool isLoaded(int handle);

private:
    std::string asset_dir;
    framework64::ShaderCache& shader_cache;

    std::unordered_map<int, std::unique_ptr<fw64Texture>> textures;
    std::unordered_map<int, std::unique_ptr<fw64Font>> fonts;
    std::unordered_map<int, std::unique_ptr<fw64Mesh>> meshes;

    sqlite3* database = nullptr;
    sqlite3_stmt* select_texture_statement = nullptr;
    sqlite3_stmt* select_font_statement = nullptr;
    sqlite3_stmt* select_mesh_statement = nullptr;
};


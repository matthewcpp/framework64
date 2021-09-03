#pragma once

#include "framework64/assets.h"
#include "framework64/desktop/mesh_renderer.h"

#include <sqlite3.h>

#include <string>
#include <unordered_map>
#include <memory>

class fw64Assets {
public:
    fw64Assets(std::string asset_dir_path, framework64::MeshRenderer& mr);
    bool init();

    fw64Texture* getTexture(int handle);
    fw64Font* getFont(int handle);
    fw64Mesh* getMesh(int handle);

    bool isLoaded(int handle);

private:
    std::string asset_dir;
    framework64::MeshRenderer& mesh_renderer;

    std::unordered_map<int, std::unique_ptr<fw64Texture>> textures;
    std::unordered_map<int, std::unique_ptr<fw64Font>> fonts;
    std::unordered_map<int, std::unique_ptr<fw64Mesh>> meshes;

    sqlite3* database;
    sqlite3_stmt* select_texture_statement;
    sqlite3_stmt* select_font_statement;
    sqlite3_stmt* select_mesh_statement;
};


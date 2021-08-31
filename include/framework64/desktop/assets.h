#pragma once

#include "framework64/assets.h"

#include <sqlite3.h>

#include <string>
#include <unordered_map>
#include <memory>

class fw64Assets {
public:
    fw64Assets(std::string asset_dir_path);
    bool init();

    fw64Texture* getTexture(int handle);
    fw64Font* getFont(int handle);

    bool isLoaded(int handle);

private:
    std::string asset_dir;

    std::unordered_map<int, std::unique_ptr<fw64Texture>> textures;
    std::unordered_map<int, std::unique_ptr<fw64Font>> fonts;

    sqlite3* database;
    sqlite3_stmt* select_texture_statement;
    sqlite3_stmt* select_font_statement;
};


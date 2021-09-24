#pragma once

#include "framework64/asset_database.h"
#include "framework64/desktop/audio_bank.h"
#include "framework64/desktop/shader_cache.h"

#include <sqlite3.h>

#include <memory>
#include <string>
#include <unordered_map>


class fw64AssetDatabase {
public:
    fw64AssetDatabase(std::string const & dir, framework64::ShaderCache& sc)
        : asset_dir(dir), shader_cache(sc) {}

    bool init(std::string const & database_path);

    std::string asset_dir;
    framework64::ShaderCache& shader_cache;

    sqlite3* database = nullptr;
    sqlite3_stmt* select_image_statement = nullptr;
    sqlite3_stmt* select_font_statement = nullptr;
    sqlite3_stmt* select_mesh_statement = nullptr;
    sqlite3_stmt* select_sound_bank_statement = nullptr;
    sqlite3_stmt* select_music_bank_statement = nullptr;
    sqlite3_stmt* select_raw_file_statement = nullptr;
};


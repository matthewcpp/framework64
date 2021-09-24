#pragma once

#include "framework64/asset_database.h"
#include "framework64/desktop/audio_bank.h"
#include "framework64/desktop/database.h"
#include "framework64/desktop/shader_cache.h"

#include <sqlite3.h>

#include <string>
#include <unordered_map>
#include <memory>

class fw64AssetDatabase {
public:
    fw64AssetDatabase(std::string const & dir, framework64::Database& db, framework64::ShaderCache& sc)
        : asset_dir(dir), database(db), shader_cache(sc) {}

    fw64SoundBank* getSoundBank(int handle);
    fw64MusicBank* getMusicBank(int handle);

    std::string asset_dir;
    framework64::ShaderCache& shader_cache;
    framework64::Database& database;

    std::unordered_map<int, std::unique_ptr<fw64SoundBank>> sound_banks;
    std::unordered_map<int, std::unique_ptr<fw64MusicBank>> music_banks;
};


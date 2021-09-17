#pragma once

#include "framework64/assets.h"
#include "framework64/desktop/database.h"
#include "framework64/desktop/shader_cache.h"

#include <string>
#include <unordered_map>
#include <memory>

class fw64Assets {
public:
    fw64Assets(std::string const & dir, framework64::Database& db, framework64::ShaderCache& sc)
        : asset_dir(dir), database(db), shader_cache(sc) {}

    fw64Texture* getTexture(int handle);
    fw64Font* getFont(int handle);
    fw64Mesh* getMesh(int handle);
    fw64SoundBank* getSoundBank(int handle);
    fw64MusicBank* getMusicBank(int handle);

private:
    std::string asset_dir;
    framework64::ShaderCache& shader_cache;
    framework64::Database& database;

    std::unordered_map<int, std::unique_ptr<fw64Texture>> textures;
    std::unordered_map<int, std::unique_ptr<fw64Font>> fonts;
    std::unordered_map<int, std::unique_ptr<fw64Mesh>> meshes;
    std::unordered_map<int, std::unique_ptr<fw64SoundBank>> sound_banks;
    std::unordered_map<int, std::unique_ptr<fw64MusicBank>> music_banks;
};


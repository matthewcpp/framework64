#pragma once

#include "framework64/audio_bank.h"

#include <SDL2/SDL_mixer.h>

#include <string>
#include <vector>

struct fw64SoundBank {
    bool load(std::string base_path, uint32_t count);

    uint32_t sound_count;
    std::string base_path;

    static fw64SoundBank* loadFromDatabase(fw64AssetDatabase* database, uint32_t index);
};

struct fw64MusicBank {
    bool load(std::string path, uint32_t count);

    uint32_t track_count = 0;
    std::string base_path;

    static fw64MusicBank* loadFromDatabase(fw64AssetDatabase* database, uint32_t index);
};

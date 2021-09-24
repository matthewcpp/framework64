#pragma once

#include "framework64/audio_bank.h"

#include <SDL2/SDL_mixer.h>

#include <string>
#include <vector>

struct fw64SoundBank {
    fw64SoundBank(std::string path, uint32_t count) : base_path(path), sound_count(count) {}
    static fw64SoundBank* loadFromDatabase(fw64AssetDatabase* database, uint32_t index);

    uint32_t sound_count;
    std::string base_path;
};

struct fw64MusicBank {
    fw64MusicBank(std::string path, uint32_t count) : base_path(path), track_count(count) {}
    static fw64MusicBank* loadFromDatabase(fw64AssetDatabase* database, uint32_t index);

    uint32_t track_count = 0;
    std::string base_path;
};

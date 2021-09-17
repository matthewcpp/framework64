#include "framework64/desktop/audio_bank.h"

bool fw64SoundBank::load(std::string path, uint32_t count) {
    base_path = std::move(path);
    sound_count = count;

    return true;
}

bool fw64MusicBank::load(std::string path, uint32_t count){
    track_count = count;
    base_path = path;
    return true;
}

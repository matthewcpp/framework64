#include "framework64/desktop/audio_bank.h"

bool fw64SoundBank::load(std::string base_path, uint32_t count) {
    sounds.resize(count, nullptr);

    for (size_t i = 0; i < sounds.size(); i++) {
        std::string sound_path = base_path + std::to_string(i) + ".ogg";

        auto* sound_effect = Mix_LoadWAV(sound_path.c_str());

        if (!sound_effect) {
            unload();
            return false;
        }

        sounds[i] = sound_effect;
    }

    return true;
}

void fw64SoundBank::unload() {
    for (const auto sound_effect : sounds) {
        if (sound_effect)
            Mix_FreeChunk(sound_effect);
    }

    sounds.resize(0);
}

bool fw64MusicBank::load(std::string base_path, uint32_t count){
    return true;
}

void fw64MusicBank::unload() {

}

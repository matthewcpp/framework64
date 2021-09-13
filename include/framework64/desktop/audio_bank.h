#pragma once

#include <SDL2/SDL_mixer.h>

#include <string>
#include <vector>

struct fw64SoundBank {
    std::vector<Mix_Chunk *> sounds;

    void unload();
    bool load(std::string base_path, uint32_t count);
};

#pragma once

#include "framework64/audio.h"
#include "framework64/desktop/audio_bank.h"

#include <string>

struct fw64Audio {
public:
    void setSoundBank(fw64SoundBank* sb);
    int playSound(uint32_t sound_num);
    void stopSound(uint32_t channel_num);
    fw64AudioStatus getChannelStatus(uint32_t channel_num);
    size_t soundCount();

private:
    fw64SoundBank* sound_bank = nullptr;
};

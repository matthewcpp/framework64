#ifndef FW64_N64_AUDIO_BANK_H
#define FW64_N64_AUDIO_HANK_H

#include "framework64/audio_bank.h"

#include <stdint.h>

struct fw64SoundBank {
    uint32_t index;
};

fw64SoundBank* fw64_n64_sound_bank_load(uint32_t index);

#endif
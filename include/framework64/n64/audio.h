#ifndef FW64_N64_AUDIO_H
#define FW64_N64_AUDIO_H

#include "framework64/audio.h"

#include <stdint.h>

struct fw64Audio {
    float music_volume;

    fw64SoundBank* sound_bank;
    fw64MusicBank* music_bank;
};

void fw64_n64_audio_init(fw64Audio* audio);

#endif
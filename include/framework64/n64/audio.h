#ifndef FW64_N64_AUDIO_H
#define FW64_N64_AUDIO_H

#include <stdint.h>

struct fw64Audio {
    uint32_t sound_count;
    uint32_t music_track_count;
    float music_volume;

    int current_sound_bank;
    int current_music_bank;
};

void fw64_n64_audio_init(fw64Audio* audio);

#endif
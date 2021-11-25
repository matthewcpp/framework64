#ifndef FW64_N64_AUDIO_H
#define FW64_N64_AUDIO_H

#include "framework64/audio.h"

#include <nusys.h>

#pragma GCC diagnostic ignored "-Wcomment"
#include <nualsgi_n.h>
#pragma GCC diagnostic pop

#include <stdint.h>

struct fw64Audio {
    float music_volume;

    fw64SoundBank* sound_bank;
    fw64MusicBank* music_bank;

    int default_music_tempo;
    float music_playback_speed;
    fw64AudioStatus music_status;
};

void fw64_n64_audio_init(fw64Audio* audio);
void fw64_n64_audio_update(fw64Audio* audio);

#endif
#ifndef FW64_AUDIO_H
#define FW64_AUDIO_H

/** \file audio.h */

#include "framework64/audio_bank.h"

#include <limits.h>
#include <stdint.h>

#define FW64_NO_AUDIO_BANK_LOADED INT_MAX

typedef enum {
    FW64_AUDIO_STOPPED = 0, //AL_STOPPED
    FW64_AUDIO_PLAYING = 1 // AL_PLAYING,
} fw64AudioStatus;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_audio_set_sound_bank(fw64Audio* audio, fw64SoundBank* sound_bank);
int fw64_audio_sound_count(fw64Audio* audio);
int fw64_audio_play_sound(fw64Audio* audio, uint32_t sound_num);
void fw64_audio_stop_sound(fw64Audio* audio, int handle);
fw64AudioStatus fw64_audio_get_sound_status(fw64Audio* audio, int handle);

void fw64_audio_set_music_bank(fw64Audio* audio, fw64MusicBank* music_bank);
int fw64_audio_play_music(fw64Audio* audio, uint32_t track_num);
void fw64_audio_stop_music(fw64Audio* audio);
void fw64_audio_set_music_volume(fw64Audio* audio, float volume);
int fw64_audio_music_track_count(fw64Audio* audio);
float fw64_audio_get_playback_speed(fw64Audio* audio);
void fw64_audio_set_music_playback_speed(fw64Audio* audio, float speed);
fw64AudioStatus fw64_audio_get_music_status(fw64Audio* audio);

#ifdef __cplusplus
}
#endif

#endif

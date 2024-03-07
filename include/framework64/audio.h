#ifndef FW64_AUDIO_H
#define FW64_AUDIO_H

/** \file audio.h */

#include <limits.h>
#include <stdint.h>

#include "framework64/asset_database.h"

typedef struct fw64Audio fw64Audio;

#define FW64_NO_AUDIO_BANK_LOADED INT_MAX

typedef enum {
    FW64_AUDIO_STOPPED = 0, // AL_STOPPED
    FW64_AUDIO_PLAYING = 1, // AL_PLAYING
} fw64AudioStatus;

#ifdef __cplusplus
extern "C" {
#endif

int fw64_audio_load_soundbank_asset(fw64Audio* audio, fw64AssetDatabase* asset_database, fw64AssetId asset_id);
void fw64_audio_unload_soundbank(fw64Audio* audio);

int fw64_audio_sound_count(fw64Audio* audio);
int fw64_audio_play_sound(fw64Audio* audio, uint32_t sound_num);
void fw64_audio_stop_sound(fw64Audio* audio, int handle);
fw64AudioStatus fw64_audio_get_sound_status(fw64Audio* audio, int handle);

int fw64_audio_load_musicbank_asset(fw64Audio* audio, fw64AssetDatabase* asset_database, fw64AssetId asset_id);
void fw64_audio_unload_musicbank(fw64Audio* audio);

/**
 * @brief Start playing a new music track. 
 * If the track passed in is currently playing then no action is taken.
 * Any currently playing track is stopped.
 * 
 * @param audio audio manager pointer
 * @param track_num index of the track to play
 * @return int non zero value indicates the new music track was successfully started.
 */
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

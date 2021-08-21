#ifndef FW64_AUDIO_H
#define FW64_AUDIO_H

#include <limits.h>
#include <stdint.h>

typedef struct fw64Audio fw64Audio;

#define FW64_NO_AUDIO_BANK_LOADED INT_MAX

typedef enum {
    FW64_AUDIO_STOPPED = 0, //AL_STOPPED
    FW64_AUDIO_PLAYING = 1, // AL_PLAYING,
    FW64_AUDIO_STOPPING = 2 // AL_STOPPING
} fw64AudioStatus;

/**
 * Loads a sound effect bank.  The number of sounds contained in the bank will be placed in audio->sound_count.
 * \return nonzero value if the bank was successfully loaded, otherwise zero if an error occurred.
*/
int fw64_audio_load_soundbank(fw64Audio* audio, int asset_id);
int fw64_audio_sound_count(fw64Audio* audio);

int fw64_audio_play_sound(fw64Audio* audio, uint32_t sound_num);
int fw64_audio_stop_sound(fw64Audio* audio, int handle);
fw64AudioStatus fw64_audio_get_sound_status(fw64Audio* audio, int handle);

/**
 * Loads a music (sequence) bank.  The number of tracks (sequences) contained in the bank will be placed in audio->music_track_count.
 * \returns 1 if the bank was successfully loaded, zero if the bank failed to load, or -1 if the bank is already loaded
 */
int fw64_audio_load_music(fw64Audio* audio, int asset_id);
int fw64_audio_play_music(fw64Audio* audio, uint32_t track_num);
int fw64_audio_stop_music(fw64Audio* audio);
void fw64_audio_set_music_volume(fw64Audio* audio, float volume);
int fw64_audio_music_track_count(fw64Audio* audio);
fw64AudioStatus fw64_audio_get_music_status(fw64Audio* audio);
#endif
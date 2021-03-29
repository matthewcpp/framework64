#ifndef FW64_AUDIO_H
#define FW64_AUDIO_H

#include <limits.h>
#include <stdint.h>

typedef struct {
    uint32_t sound_count;
    uint32_t music_track_count;
    float music_volume;

    int current_sound_bank;
    int current_music_bank;
} Audio;

typedef enum {
    SOUND_STATE_STOPPED,
    SOUND_STATE_PLAYING
} SoundStatus;

#define FW64_NO_AUDIO_BANK_LOADED INT_MAX

void audio_init(Audio* audio);

/**
 * Loads a sound effect bank.  The number of sounds contained in the bank will be placed in audio->sound_count.
 * \return nonzero value if the bank was successfully loaded, otherwise zero if an error occurred.
*/
int audio_load_soundbank(Audio* audio, int asset_id);

int audio_play_sound(Audio* audio, uint32_t sound_num);
int audio_stop_sound(Audio* audio, int handle);
SoundStatus audio_get_sound_status(Audio* audio, int handle);

/**
 * Loads a music (sequence) bank.  The number of tracks (sequences) contained in the bank will be placed in audio->music_track_count.
 * \returns 1 if the bank was successfully loaded, zero if the bank failed to load, or -1 if the bank is already loaded
 */
int audio_load_music(Audio* audio, int asset_id);
int audio_play_music(Audio* audio, uint32_t track_num);
int audio_stop_music(Audio* audio);
void audio_set_music_volume(Audio* audio, float volume);
#endif
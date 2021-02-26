#ifndef FW64_AUDIO_H
#define FW64_AUDIO_H

#include <stdint.h>

typedef struct {
    uint32_t sound_count;
} Audio;

typedef enum {
    SOUND_STATE_STOPPED,
    SOUND_STATE_PLAYING
} SoundStatus;

void audio_init(Audio* audio);
int audio_load_soundbank(Audio* audio, int asset_id);
int audio_play_sound(Audio* audio, uint32_t sound_num);
int audio_stop_sound(Audio* audio, int handle);
SoundStatus audio_get_sound_status(Audio* audio, int handle);

#endif
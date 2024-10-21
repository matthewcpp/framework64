#include "libdragon_audio.h"

int fw64_audio_load_soundbank_asset(fw64Audio* audio, fw64AssetDatabase* asset_database, fw64AssetId asset_id) {
    audio->audio_track;
    wav64_open(&audio->audio_track, asset_id);

    (void)asset_database;
}

#define CHANNEL 1

int fw64_audio_play_sound(fw64Audio* audio, uint32_t sound_num) {
    mixer_ch_play(CHANNEL, &audio->audio_track.wave);
    (void)sound_num;

    return CHANNEL;
}

fw64AudioStatus fw64_audio_get_sound_status(fw64Audio* audio, int handle) {
    return mixer_ch_playing(handle) ? FW64_AUDIO_PLAYING : FW64_AUDIO_STOPPED;
}

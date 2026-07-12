#include "libdragon_audio.h"

#include "framework64/n64_libdragon/libdragon_asset_database.h"

int fw64_audio_load_soundbank_asset(fw64Audio* audio, fw64AssetDatabase* asset_database, fw64AssetId asset_id) {
    audio->audio_track;

    char audio_path[16];
    fw64_libdragon_asset_database_filepath(asset_id, audio_path);
    wav64_open(&audio->audio_track, audio_path);

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

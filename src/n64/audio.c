#include "framework64/n64/audio.h"
#include "framework64/n64/filesystem.h"
#include "framework64/n64/audio_bank.h"

#include <nusys.h>

#pragma GCC diagnostic ignored "-Wcomment"
#include <nualsgi_n.h>
#pragma GCC diagnostic pop

#include <string.h>

#define FW64_N64_MAX_VOL 0x7fff

void fw64_n64_audio_init(fw64Audio* audio) {
    memset(audio, 0, sizeof(audio));
    audio->music_volume = 1.0f;
    audio->sound_bank = NULL;
    audio->music_bank = NULL;
}

void fw64_audio_set_sound_bank(fw64Audio* audio, fw64SoundBank* sound_bank) {
    nuAuSndPlayerBankSet((u8*)sound_bank->ctrl_file_address, sound_bank->ctrl_file_size, (u8*)sound_bank->tbl_file_address);
    audio->sound_bank = sound_bank;
}

int fw64_audio_play_sound(fw64Audio* audio, uint32_t sound_num) {
    if (sound_num < fw64_audio_sound_count(audio)) {
        ALSndId sound_id = nuAuSndPlayerPlay(sound_num);
        return (int)sound_id;
    }
    else {
        return -1;
    }
}

void fw64_audio_stop_sound(fw64Audio* audio, int handle) {
    (void)audio;
    nuAuSndPlayerSetSound(handle);
    nuAuSndPlayerStop();
}

fw64AudioStatus fw64_audio_get_sound_status(fw64Audio* audio, int handle) {
    (void)audio;
    nuAuSndPlayerSetSound(handle);
    return (fw64AudioStatus)nuAuSndPlayerGetState();
}

int fw64_audio_sound_count(fw64Audio* audio) {
    if (audio->sound_bank)
        return audio->sound_bank->song_count;
    else
        return 0;
}

void fw64_audio_set_music_bank(fw64Audio* audio, fw64MusicBank* music_bank) {
    fw64_audio_stop_music(audio);

    nuAuSeqPlayerBankSet((u8*)music_bank->ctrl_file_address, music_bank->ctrl_file_size, (u8*)music_bank->tbl_file_address);
    nuAuSeqPlayerSeqSet((u8*)music_bank->seq_file_address);

    audio->music_bank = music_bank;
}

int fw64_audio_play_music(fw64Audio* audio, uint32_t track_num) {
    if (track_num < fw64_audio_music_track_count(audio)) {
        nuAuSeqPlayerStop(0);
        nuAuSeqPlayerSetNo(0, track_num);
        nuAuSeqPlayerPlay(0);

        return 1;
    }

    return 0;
}

void fw64_audio_stop_music(fw64Audio* audio){
    nuAuSeqPlayerStop(0);
}

void fw64_audio_set_music_volume(fw64Audio* audio, float volume) {
    audio->music_volume = volume;
    nuAuSeqPlayerSetVol(0, (s16)(volume * FW64_N64_MAX_VOL));
}

fw64AudioStatus fw64_audio_get_music_status(fw64Audio* audio) {
    (void)audio;
    return (fw64AudioStatus)nuAuSeqPlayerGetState(0);
}

int fw64_audio_music_track_count(fw64Audio* audio) {
    if (audio->music_bank)
        return audio->music_bank->track_count;
    else
        return 0;
}

#include "framework64/n64/audio.h"
#include "framework64/n64/filesystem.h"
#include "framework64/n64/audio_bank.h"

#include <string.h>

#define FW64_N64_MAX_VOL 0x7fff

void fw64_n64_audio_init(fw64Audio* audio) {
    memset(audio, 0, sizeof(audio));
    audio->music_volume = 1.0f;
    audio->sound_bank = NULL;
    audio->music_bank = NULL;
    audio->music_status = FW64_AUDIO_STOPPED;
    audio->default_music_tempo = 0;
    audio->music_playback_speed = 1.0f;
}

void fw64_n64_audio_update(fw64Audio* audio) {
    int sequence_player_status = nuAuSeqPlayerGetState(0);
    
    if (audio->music_status == FW64_AUDIO_PLAYING) {
        // the sequencer just started playing
        if (audio->default_music_tempo == 0 && sequence_player_status == AL_PLAYING) {
            audio->default_music_tempo = nuAuSeqPlayerGetTempo(0);
            nuAuSeqPlayerSetTempo(0, (int)(audio->default_music_tempo * (1.0f / audio->music_playback_speed)));
        }

        // the sequencer just stopped
        if (sequence_player_status == AL_STOPPED && audio->default_music_tempo != 0) {
            audio->default_music_tempo = 0;
            audio->music_status = FW64_AUDIO_STOPPED;
        }
    }
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
        audio->music_status = FW64_AUDIO_PLAYING;
        
        return 1;
    }

    return 0;
}

void fw64_audio_stop_music(fw64Audio* audio){
    nuAuSeqPlayerStop(0);
    audio->music_status = FW64_AUDIO_STOPPED;
    audio->default_music_tempo = 0;
}

void fw64_audio_set_music_volume(fw64Audio* audio, float volume) {
    audio->music_volume = volume;
    nuAuSeqPlayerSetVol(0, (s16)(volume * FW64_N64_MAX_VOL));
}

fw64AudioStatus fw64_audio_get_music_status(fw64Audio* audio) {
    return audio->music_status;
}

int fw64_audio_music_track_count(fw64Audio* audio) {
    if (audio->music_bank)
        return audio->music_bank->track_count;
    else
        return 0;
}

float fw64_audio_get_playback_speed(fw64Audio* audio) {
    return audio->music_playback_speed;
}

void fw64_audio_set_music_playback_speed(fw64Audio* audio, float speed) {
    if (speed <= 0.0f) return;

    audio->music_playback_speed = speed;

    if (audio->music_status == FW64_AUDIO_PLAYING) {
        nuAuSeqPlayerSetTempo(0, (int)(audio->default_music_tempo * (1.0f / audio->music_playback_speed)));
    }
}
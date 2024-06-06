#include "framework64/n64_libultra/audio.h"
#include "framework64/n64_libultra/filesystem.h"

#include <string.h>

#define FW64_N64_MAX_VOL 0x7fff

void fw64_n64_audio_init(fw64Audio* audio) {
    audio->music_volume = 1.0f;

    memset(&audio->sound_bank, 0, sizeof(fw64SoundBank));
    memset(&audio->music_bank, 0, sizeof(fw64MusicBank));

    audio->default_music_tempo = 0;
    audio->music_playback_speed = 1.0f;
    audio->music_status = FW64_AUDIO_STOPPED;

    audio->active_sound_channel_count = 0;
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

    for (int i = 0; i < audio->active_sound_channel_count; i++) {
        nuAuSndPlayerSetSound(audio->active_sound_channels[i]);
        s32 state =  nuAuSndPlayerGetState();
        if (state != AL_PLAYING) {
            audio->active_sound_channels[i] = audio->active_sound_channel_count - 1;
            audio->active_sound_channel_count -=1;
            i -= 1;
        }
    }
}

typedef struct {
    uint32_t song_count;
    uint32_t ctrl_file_size;
} fw64SoundbankHeader;

typedef struct {
    uint32_t track_count;
    uint32_t bank_asset_id;
    uint32_t sequence_bank_size; // this is currently unused
} fw64MusicHeader;

int fw64_audio_load_soundbank_asset(fw64Audio* audio, fw64AssetDatabase*, fw64AssetId asset_id) {
    int handle = fw64_filesystem_open(asset_id);

    if (handle < 0) {
        return 0;
    }

    fw64SoundbankHeader header;
    fw64_filesystem_read(&header, sizeof(fw64SoundbankHeader), 1, handle);
    fw64_filesystem_close(handle);

    uint32_t rom_address = fw64_n64_filesystem_get_rom_address(asset_id);
    uint32_t ctrl_file_address = rom_address + sizeof(fw64SoundbankHeader);
    uint32_t tbl_file_address = ctrl_file_address + header.ctrl_file_size;

    fw64SoundBank* sound_bank = &audio->sound_bank;

    sound_bank->song_count = header.song_count;
    sound_bank->ctrl_file_size = header.ctrl_file_size;
    sound_bank->ctrl_file_address = ctrl_file_address;
    sound_bank->tbl_file_address = tbl_file_address;
        
    nuAuSndPlayerBankSet((u8*)sound_bank->ctrl_file_address, sound_bank->ctrl_file_size, (u8*)sound_bank->tbl_file_address);
    
    return 1;
}

void fw64_audio_unload_soundbank(fw64Audio* audio) {
    audio->sound_bank.song_count = 0;
}

int fw64_audio_play_sound(fw64Audio* audio, uint32_t sound_num) {
    if (sound_num < fw64_audio_sound_count(audio) && audio->active_sound_channel_count < N64_LIBULTRA_MAX_ACTIVE_SOUND_COUNT) {
        ALSndId sound_id = nuAuSndPlayerPlay(sound_num);
        // this is not totally ideal, however sounds do not seem to go into a playing state immediately after calling nuAuSndPlayerPlay
        audio->active_sound_channels[audio->active_sound_channel_count++] = sound_id;
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

    for (int i = 0; i < audio->active_sound_channel_count; i++) {
        if (audio->active_sound_channels[i] == handle) {
            audio->active_sound_channels[i] = audio->active_sound_channel_count - 1;
            audio->active_sound_channel_count -=1;
            break;
        }
    }
}

fw64AudioStatus fw64_audio_get_sound_status(fw64Audio* audio, int handle) {
    for (int i = 0; i < audio->active_sound_channel_count; i++) {
        if (audio->active_sound_channels[i] == handle){
            return FW64_AUDIO_PLAYING;
        }
    }

    return FW64_AUDIO_STOPPED;
}

uint32_t fw64_audio_sound_count(fw64Audio* audio) {
    return audio->sound_bank.song_count;

}

int fw64_audio_load_musicbank_asset(fw64Audio* audio, fw64AssetDatabase*, fw64AssetId asset_id) {
    // load the music bank header which contains sequence info and also the asset ID of the sound bank
    int handle = fw64_filesystem_open(asset_id);

    if (handle < 0)
        return 0;

    fw64MusicHeader music_header;
    fw64_filesystem_read(&music_header, sizeof(fw64MusicHeader), 1, handle);
    fw64_filesystem_close(handle);

    // get the rom address of the sequence file which contains the compressed midi files
    uint32_t rom_address = fw64_n64_filesystem_get_rom_address(asset_id);
    uint32_t seq_file_address = rom_address + sizeof(fw64MusicHeader);

    // read the associated Sound bank headerfor the music
    handle = fw64_filesystem_open(music_header.bank_asset_id);

    if (handle < 0)
        return 0;

    fw64SoundbankHeader soundbank_header;
    fw64_filesystem_read(&soundbank_header, sizeof(fw64SoundbankHeader), 1, handle);
    fw64_filesystem_close(handle);

    // calculate the offsets for the ctrl & table file which hold the MIDI note / sample data
    rom_address = fw64_n64_filesystem_get_rom_address(music_header.bank_asset_id);
    uint32_t ctrl_file_address = rom_address + sizeof(fw64SoundbankHeader);
    uint32_t tbl_file_address = ctrl_file_address + soundbank_header.ctrl_file_size;

    fw64MusicBank* music_bank = &audio->music_bank;
    music_bank->track_count = music_header.track_count;
    music_bank->seq_file_address = seq_file_address;

    music_bank->ctrl_file_address = ctrl_file_address;
    music_bank->ctrl_file_size = soundbank_header.ctrl_file_size;
    music_bank->tbl_file_address = tbl_file_address;

    fw64_audio_stop_music(audio);
    nuAuSeqPlayerBankSet((u8*)music_bank->ctrl_file_address, music_bank->ctrl_file_size, (u8*)music_bank->tbl_file_address);
    nuAuSeqPlayerSeqSet((u8*)music_bank->seq_file_address);

    return 1;
}

void fw64_audio_unload_musicbank(fw64Audio* audio) {
    fw64_audio_stop_music(audio);
    audio->music_bank.track_count = 0;
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

uint32_t fw64_audio_music_track_count(fw64Audio* audio) {
    return audio->music_bank.track_count;
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
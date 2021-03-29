#include "framework64/audio.h"

#include "framework64/filesystem.h"

#include <nusys.h>

#pragma GCC diagnostic ignored "-Wcomment"
#include <nualsgi_n.h>
#pragma GCC diagnostic pop

#include <string.h>

typedef struct {
    uint32_t song_count;
    uint32_t ctrl_file_size;
} SoundbankHeader;

typedef struct {
    uint32_t track_count;
    uint32_t bank_asset_id;
    uint32_t sequence_bank_size;
} MusicHeader;

void audio_init(Audio* audio) {
    memset(audio, 0, sizeof(audio));
    audio->music_volume = 1.0f;
    audio->current_sound_bank = FW64_NO_AUDIO_BANK_LOADED;
    audio->current_music_bank = FW64_NO_AUDIO_BANK_LOADED;
}

typedef enum {
    BANK_TYPE_SOUND,
    BANK_TYPE_SEQUENCE
} BankType;

static int load_bank(Audio* audio, BankType bank_type, int asset_id) {
    int handle = filesystem_open(asset_id);

    if (handle < 0)
        return handle;

    SoundbankHeader header;
    filesystem_read(&header, sizeof(SoundbankHeader), 1, handle);
    filesystem_close(handle);

    uint32_t rom_address = n64_filesystem_get_rom_address(asset_id);
    uint32_t ctrl_file_address = rom_address + sizeof(SoundbankHeader);
    uint32_t tbl_file_address = ctrl_file_address + header.ctrl_file_size;

    if (bank_type == BANK_TYPE_SOUND) {
        audio->sound_count = header.song_count;
        nuAuSndPlayerBankSet((u8*)ctrl_file_address, header.ctrl_file_size, (u8*)tbl_file_address);
    }
    else
        // note that in the sequence count the song_count value of the header file is irreverent
        nuAuSeqPlayerBankSet((u8*)ctrl_file_address, header.ctrl_file_size, (u8*)tbl_file_address);

    return 1;
}

int audio_load_soundbank(Audio* audio, int asset_id) {
    if (asset_id == audio->current_sound_bank)
        return 0;

    int result = load_bank(audio, BANK_TYPE_SOUND, asset_id);

    if (result == 1)
        audio->current_sound_bank = asset_id;

    return result;
}

int audio_play_sound(Audio* audio, uint32_t sound_num) {
    if (sound_num < audio->sound_count) {
        ALSndId sound_id = nuAuSndPlayerPlay(sound_num);
        return (int)sound_id;
    }
    else {
        return -1;
    }
}

int audio_stop_sound(Audio* audio, int handle) {
    (void)audio;
    nuAuSndPlayerSetSound(handle);
    nuAuSndPlayerStop();

    return 1;
}

fw64AudioStatus audio_get_sound_status(Audio* audio, int handle) {
    (void)audio;
    nuAuSndPlayerSetSound(handle);
    return (fw64AudioStatus)nuAuSndPlayerGetState();
}

int audio_load_music(Audio* audio, int asset_id) {
    if (asset_id == audio->current_music_bank)
        return 0;

    audio_stop_music(audio);

    int handle = filesystem_open(asset_id);

    if (handle < 0)
        return handle;

    MusicHeader header;
    filesystem_read(&header, sizeof(MusicHeader), 1, handle);
    filesystem_close(asset_id);

    int result = load_bank(audio, BANK_TYPE_SEQUENCE, header.bank_asset_id);
    if (result != 1)
        return result;

    audio->music_track_count = header.track_count;

    uint32_t rom_address = n64_filesystem_get_rom_address(asset_id);
    uint32_t seq_file_address = rom_address + sizeof(MusicHeader);
    nuAuSeqPlayerSeqSet((u8*)seq_file_address);

    audio->current_music_bank = asset_id;

    return 1;
}

int audio_play_music(Audio* audio, uint32_t track_num) {
    nuAuSeqPlayerStop(0);
    nuAuSeqPlayerSetNo(0, track_num);
    nuAuSeqPlayerPlay(0);
    return 1;
}

int audio_stop_music(Audio* audio){
    nuAuSeqPlayerStop(0);
    return 0;
}

#define MAX_VOL 0x7fff

void audio_set_music_volume(Audio* audio, float volume) {
    audio->music_volume = volume;
    nuAuSeqPlayerSetVol(0, (s16)(volume * MAX_VOL));
}

fw64AudioStatus audio_get_music_status(Audio* audio) {
    (void)audio;
    return (fw64AudioStatus)nuAuSeqPlayerGetState(0);
}

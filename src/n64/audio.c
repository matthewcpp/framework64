#include "framework64/audio.h"

#include "framework64/filesystem.h"

#include <nusys.h>

#pragma GCC diagnostic ignored "-Wcomment"
#include <nualsgi_n.h>
#pragma GCC diagnostic pop

typedef struct {
    uint32_t song_count;
    uint32_t ctrl_file_size;
} SoundbankHeader;

void audio_init(Audio* audio) {
    audio->sound_count = 0;
}

int audio_load_soundbank(Audio* audio, int asset_id) {
    int handle = filesystem_open(asset_id);
    SoundbankHeader header;
    filesystem_read(&header, sizeof(SoundbankHeader), 1, handle);
    filesystem_close(asset_id);

    audio->sound_count = header.song_count;

    uint32_t rom_address = n64_filesystem_get_rom_address(asset_id);
    uint32_t ctrl_file_address = rom_address + sizeof(SoundbankHeader);
    uint32_t tbl_file_address = ctrl_file_address + header.ctrl_file_size;
    (void)tbl_file_address;
    nuAuSndPlayerBankSet((u8*)ctrl_file_address, header.ctrl_file_size, (u8*)tbl_file_address);

    return 1;
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

SoundStatus audio_get_sound_status(Audio* audio, int handle) {
    (void)audio;
    nuAuSndPlayerSetSound(handle);
    s32 status = nuAuSndPlayerGetState();

    if (status == AL_PLAYING)
        return SOUND_STATE_PLAYING;
    else
        return SOUND_STATE_STOPPED;
}
#ifndef FW64_N64_AUDIO_BANK_H
#define FW64_N64_AUDIO_HANK_H

#include "framework64/audio_bank.h"

#include <stdint.h>

struct fw64SoundBank {
    uint32_t ctrl_file_address;
    uint32_t ctrl_file_size;
    uint32_t tbl_file_address;
    uint32_t song_count;
    
};

fw64SoundBank* fw64_n64_sound_bank_load(uint32_t index);

struct fw64MusicBank {
    uint32_t track_count;
    uint32_t seq_file_address;
    uint32_t ctrl_file_address;
    uint32_t ctrl_file_size;
    uint32_t tbl_file_address;
};

fw64MusicBank* fw64_n64_music_bank_load(uint32_t index);

#endif
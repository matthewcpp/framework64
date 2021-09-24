#pragma once

#include "framework64/audio_bank.h"

#include <stdint.h>

struct fw64SoundBank {
    uint32_t ctrl_file_address;
    uint32_t ctrl_file_size;
    uint32_t tbl_file_address;
    uint32_t song_count;
    
};

struct fw64MusicBank {
    uint32_t track_count;
    uint32_t seq_file_address;
    uint32_t ctrl_file_address;
    uint32_t ctrl_file_size;
    uint32_t tbl_file_address;
};

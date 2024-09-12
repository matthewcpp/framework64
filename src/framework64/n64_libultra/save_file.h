#pragma once

#include "framework64/save_file.h"

#include <nusys.h>

struct fw64SaveFile {
    s32 eeprom_type;

    u8* write_buffer;
    uint32_t write_base_addr;
    uint32_t write_size;
};

void fw64_n64_save_file_init(fw64SaveFile* save_file);
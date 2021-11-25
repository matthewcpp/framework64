#pragma once

#include "framework64/filesystem.h"

#include <nusys.h>

int fw64_n64_filesystem_init(int asset_count);
uint32_t fw64_n64_filesystem_get_rom_address(int index);

int fw64_n64_filesystem_raw_read(void* buffer, u32 rom_location, u32 size);

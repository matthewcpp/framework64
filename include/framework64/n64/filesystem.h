#pragma once

#include "framework64/filesystem.h"

int fw64_n64_filesystem_init(int asset_count);
uint32_t fw64_n64_filesystem_get_rom_address(int index);

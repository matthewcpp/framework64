#pragma once

#include "framework64/filesystem.h"
#include "framework64/data_io.h"

#include <nusys.h>

extern u8 _fw64bundleSegmentRomStart[];

int fw64_n64_filesystem_init(int asset_count);

uint32_t fw64_n64_filesystem_get_rom_address(int index);

int fw64_n64_filesystem_raw_read(void* buffer, u32 rom_location, u32 size);

typedef struct {
    fw64DataSource interface;
    int file_handle;
} fw64N64FilesystemDataSource;

void fw64_n64_filesystem_datasource_init(fw64N64FilesystemDataSource* data_source, int file_handle);

int fw64_n64_filesystem_open_datasource(fw64N64FilesystemDataSource* data_source, int file_handle);
int fw64_n64_filesystem_close_datasource(fw64N64FilesystemDataSource* data_source);
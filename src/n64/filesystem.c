#include "framework64/filesystem.h"

#include <nusys.h>

#include <limits.h>
#include <malloc.h>
#include <string.h>

#define ASSET_HEADER_SIZE 4
#define MAX_DMA 16384
#define MAX_UNALIGNED_READ 256

extern u8 _asset_dataSegmentRomStart[];

typedef struct {
    uint32_t data_loc;
    uint32_t size;
    uint32_t get_pos;
} fw64FileHandle;

fw64FileHandle open_files[FW64_FILESYSTEM_MAX_OPEN_FILES];

uint32_t* asset_offsets;
int asset_count;
uint8_t read_cache[MAX_UNALIGNED_READ] __attribute__ ((aligned (8)));

int fw64_n64_filesystem_init(int num_assets) {
    asset_count = num_assets;
    asset_offsets = memalign(8, sizeof(uint32_t) * asset_count);
    memset(&open_files[0], 0, sizeof(fw64FileHandle) * FW64_FILESYSTEM_MAX_OPEN_FILES);
    nuPiReadRom((u32)(&_asset_dataSegmentRomStart[0] + 4), asset_offsets, asset_count * sizeof(uint32_t));

    return 1;
}

int fw64_filesystem_open(int asset_index) {
    if (asset_index < 0 || asset_index >= asset_count)
        return FW64_FILESYSTEM_INVALID_HANDLE;

    int file_handle = FW64_FILESYSTEM_INVALID_HANDLE;
    for (int i = 0; i < FW64_FILESYSTEM_MAX_OPEN_FILES; i++) {
        if (open_files[i].data_loc == 0) {
            file_handle = i;
            break;
        }
    }

    if (file_handle == FW64_FILESYSTEM_INVALID_HANDLE) {
        return FW64_FILESYSTEM_MAX_FILES_OPEN;
    }

    fw64FileHandle* handle = &open_files[file_handle];
    handle->data_loc = asset_offsets[asset_index] + ASSET_HEADER_SIZE;
    handle->get_pos = 0;

    // read the header info for this asset
    u32 rom_location = (u32)(&_asset_dataSegmentRomStart[0]) + asset_offsets[asset_index];
    nuPiReadRom(rom_location, &read_cache[0], ASSET_HEADER_SIZE);
    memcpy(&handle->size, &read_cache[0], sizeof(uint32_t));

    return file_handle;
}

int fw64_filesystem_size(int handle) {
    if (handle < 0 || handle > FW64_FILESYSTEM_MAX_OPEN_FILES || open_files[handle].data_loc == 0)
        return FW64_FILESYSTEM_INVALID_HANDLE;

    return open_files[handle].size;
}

int fw64_filesystem_read(void* buffer, int size, int count, int handle) {
    if (handle < 0 || handle > FW64_FILESYSTEM_MAX_OPEN_FILES || open_files[handle].data_loc == 0)
        return FW64_FILESYSTEM_INVALID_HANDLE;

    fw64FileHandle* file_handle = &open_files[handle];

    // adjust data total to prevent possible reading beyond file size;
    u32 data_total = (u32)(size * count);
    if (file_handle->get_pos + data_total >= file_handle->size) {
        data_total = file_handle->size - file_handle->get_pos;
    }

    u32 rom_location = (u32)(&_asset_dataSegmentRomStart[0]) + file_handle->data_loc + file_handle->get_pos;

    u8* dest_buffer = buffer;
    int buffer_is_aligned = ((u32)buffer & 7) == 0;
    u32 max_read_size = buffer_is_aligned ? MAX_DMA : MAX_UNALIGNED_READ;
    
    u32 data_reamining = data_total;
    while (data_reamining > 0) {
        u32 chunk_size = data_reamining <= max_read_size ? data_reamining : max_read_size;
        
        if (buffer_is_aligned) {
            nuPiReadRom(rom_location, dest_buffer, chunk_size);
        }
        else {
            nuPiReadRom(rom_location, read_cache, chunk_size);
            memcpy(dest_buffer, read_cache, chunk_size);
        }

        dest_buffer += chunk_size;
        open_files[handle].get_pos += chunk_size;
        rom_location += chunk_size;
        data_reamining -= chunk_size;
    }

    return data_total;
}

int fw64_filesystem_close(int handle) {
    if (handle < 0 || handle >= FW64_FILESYSTEM_MAX_OPEN_FILES || open_files[handle].data_loc == 0)
        return FW64_FILESYSTEM_INVALID_HANDLE;

    open_files[handle].data_loc = 0;

    return 1;
}

uint32_t fw64_n64_filesystem_get_rom_address(int asset_index) {
    return (u32)(&_asset_dataSegmentRomStart[0]) + asset_offsets[asset_index] + ASSET_HEADER_SIZE;
}

int fw64_filesystem_get_open_handle_count() {
    int count = 0;

    for (int i = 0; i < FW64_FILESYSTEM_MAX_OPEN_FILES; i++) {
        if (open_files[i].data_loc == 0) {
            count += 1;
            break;
        }
    }

    return 0;
}
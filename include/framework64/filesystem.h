#pragma once

#include <stdint.h>

#define FW64_FILESYSTEM_MAX_OPEN_FILES 4
#define FW64_FILESYSTEM_INVALID_HANDLE -1
#define FW64_FILESYSTEM_MAX_FILES_OPEN -2

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a read only file handle.
 * \param asset_index the asset index to open
 * \returns file handle with value >= 0 if the asset index was valid, otherwise a negative value indicating an error.
 * */
int fw64_filesystem_open(int asset_index);

int fw64_filesystem_size(int file_handle);
int fw64_filesystem_read(void* buffer, int size, int count, int file_handle);
int fw64_filesystem_close(int file_handle);
int fw64_filesystem_get_open_handle_count();

#ifdef __cplusplus
}
#endif

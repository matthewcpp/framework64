#ifndef FW64_FILESYSTEM_H
#define FW64_FILESYSTEM_H

#include <stdint.h>

#define FW64_FILESYSTEM_MAX_OPEN_FILES 4
#define FW64_FILESYSTEM_INVALID_HANDLE -1
#define FW64_FILESYSTEM_MAX_FILES_OPEN -2

/**
 * Creates a read only file handle.
 * \param index the asset index to open
 * \returns file handle with value >= 0 if the asset index was valid, otherwise a negative value indicating an error.
 * */
int fw64_filesystem_open(int index);

int fw64_filesystem_size(int handle);
int fw64_filesystem_read(void* buffer, int size, int count, int handle);
int fw64_filesystem_close(int handle);
int fw64_filesystem_get_open_handle_count();

#endif
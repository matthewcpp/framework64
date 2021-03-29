#ifndef FW64_FILESYSTEM_H
#define FW64_FILESYSTEM_H

#include <stdint.h>

#define FW64_FILESYSTEM_MAX_OPEN_FILES 4
#define FW64_FILESYSTEM_INVALID_HANDLE -1
#define FW64_FILESYSTEM_MAX_FILES_OPEN -2

int filesystem_init();

/**
 * Creates a read only file handle.
 * \param index the asset index to open
 * \returns file handle with value >= 0 if the asset index was valid, otherwise a negative value indicating an error.
 * */
int filesystem_open(int index);

int filesystem_size(int handle);
int filesystem_read(void* buffer, int size, int count, int handle);
int filesystem_close(int handle);
int filesystem_get_open_handle_count();
uint32_t n64_filesystem_get_rom_address(int index);

#endif
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>

#define FW64_FILESYSTEM_MAX_OPEN_FILES 4
#define FW64_FILESYSTEM_INVALID_HANDLE -1

int filesystem_init();
int filesystem_open(int index);
int filesystem_size(int handle);
int filesystem_read(void* buffer, int size, int count, int handle);
int filesystem_close(int handle);

#endif
#include "framework64/filesystem.h"
#include "framework64/desktop/filesystem.h"

int fw64_desktop_filesystem_init() {
    return 1;
}

int fw64_filesystem_open(int index) {
    return FW64_FILESYSTEM_INVALID_HANDLE;
}

int fw64_filesystem_size(int handle) {
    return FW64_FILESYSTEM_INVALID_HANDLE;
}
int fw64_filesystem_read(void* buffer, int size, int count, int handle) {
    return FW64_FILESYSTEM_INVALID_HANDLE;
}

int fw64_filesystem_close(int handle) {
    return FW64_FILESYSTEM_INVALID_HANDLE;
}
int fw64_filesystem_get_open_handle_count() {
    return FW64_FILESYSTEM_INVALID_HANDLE;
}
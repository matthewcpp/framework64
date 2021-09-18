#include "framework64/filesystem.h"
#include "framework64/desktop/filesystem.h"

#include <memory>

namespace framework64 {

std::unique_ptr<Filesystem> Filesystem::instance;

void Filesystem::init(std::string const & path, framework64::Database& database) {
    instance = std::make_unique<Filesystem>(path, database);
}

int Filesystem::getNextAvailableHandle() {
    for (size_t i = 0; i < file_handles.size(); i++) {
        if (!file_handles[i].is_open())
            return static_cast<int>(i);
    }

    return FW64_FILESYSTEM_MAX_FILES_OPEN;
}

int Filesystem::open(int asset_index) {
    int file_handle_index = getNextAvailableHandle();

    if (file_handle_index < 0)
        return file_handle_index;

    sqlite3_reset(database.select_raw_file_statement);
    sqlite3_bind_int(database.select_raw_file_statement, 1, asset_index);

    if(sqlite3_step(database.select_raw_file_statement) != SQLITE_ROW)
        return FW64_FILESYSTEM_INVALID_HANDLE;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database.select_raw_file_statement, 0));
    std::string full_path = base_path + asset_path;

    file_handles[file_handle_index].open(full_path.c_str(), std::ios::binary);
    file_sizes[file_handle_index] = sqlite3_column_int(database.select_raw_file_statement, 1);

    return file_handle_index;
}

int Filesystem::close(int file_handle) {
    if (file_handle < 0 || file_handle >= FW64_FILESYSTEM_MAX_OPEN_FILES)
        return FW64_FILESYSTEM_INVALID_HANDLE;

    auto& file = file_handles[file_handle];
    if (file.is_open()) {
        file.close();
        return 1;
    }
    else {
        return FW64_FILESYSTEM_INVALID_HANDLE;
    }
}

int Filesystem::fileSize(int file_handle) {
    if (file_handle < 0 || file_handle >= FW64_FILESYSTEM_MAX_OPEN_FILES)
        return FW64_FILESYSTEM_INVALID_HANDLE;

    if (file_handles[file_handle].is_open()) {
        return file_sizes[file_handle];
    }
    else {
        return FW64_FILESYSTEM_INVALID_HANDLE;
    }
}

int Filesystem::read(char* buffer, int count, int file_handle) {
    if (file_handle < 0 || file_handle >= FW64_FILESYSTEM_MAX_OPEN_FILES)
        return FW64_FILESYSTEM_INVALID_HANDLE;

    auto& file = file_handles[file_handle];
    if (file.is_open()) {
        file.read(buffer, count);
        return static_cast<int>(file.gcount());
    }
    else {
        return FW64_FILESYSTEM_INVALID_HANDLE;
    }
}

int Filesystem::openHandleCount() {
    int count = 0;

    for (auto const & file_handle : file_handles) {
        if (file_handle.is_open())
            count += 1;
    }

    return count;
}

}

// C Interface

int fw64_filesystem_open(int asset_index) {
    return framework64::Filesystem::get()->open(asset_index);
}

int fw64_filesystem_size(int file_handle) {
    return framework64::Filesystem::get()->fileSize(file_handle);
}
int fw64_filesystem_read(void* buffer, int size, int count, int file_handle) {
    return framework64::Filesystem::get()->read(reinterpret_cast<char*>(buffer), size * count, file_handle);
}

int fw64_filesystem_close(int file_handle) {
    return framework64::Filesystem::get()->close(file_handle);
}
int fw64_filesystem_get_open_handle_count() {
    return framework64::Filesystem::get()->openHandleCount();
}
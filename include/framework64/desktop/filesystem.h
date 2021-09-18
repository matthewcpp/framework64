#pragma once

#include "framework64/filesystem.h"
#include "framework64/desktop/database.h"

#include <array>
#include <fstream>
#include <memory>
#include <string>

namespace framework64 {

class Filesystem {
public:
    Filesystem(std::string const & path, framework64::Database& db) : base_path(path), database(db){}

    int open(int asset_index);
    int close(int file_handle);
    int fileSize(int file_handle);
    int read(char* buffer, int count, int file_handle);
    int openHandleCount();

public:
    static void init(std::string const & path, framework64::Database& database);
    static inline Filesystem* get() { return instance.get(); }

private:
    int getNextAvailableHandle();

private:
    std::string base_path;
    Database& database;
    std::array<std::ifstream, FW64_FILESYSTEM_MAX_OPEN_FILES> file_handles;
    std::array<int, FW64_FILESYSTEM_MAX_OPEN_FILES> file_sizes;

    static std::unique_ptr<Filesystem> instance;
};

}
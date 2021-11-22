#pragma once

#include "framework64/filesystem.h"
#include "framework64/desktop/asset_database.h"

#include <array>
#include <fstream>
#include <memory>
#include <string>

namespace framework64 {

class Filesystem {
public:
    Filesystem(std::string const & path, fw64AssetDatabase& db) : base_path(path), database(db){}

    int open(int asset_index);
    int openFile(const std::string& path);
    int close(int file_handle);
    int fileSize(int file_handle);
    int tell(int file_handle);
    int read(char* buffer, int count, int file_handle);
    int openHandleCount();

public:
    static void init(std::string const & path, fw64AssetDatabase& db);
    static inline Filesystem* get() { return instance.get(); }

private:
    int getNextAvailableHandle();

private:
    std::string base_path;
    fw64AssetDatabase& database;
    std::array<std::ifstream, FW64_FILESYSTEM_MAX_OPEN_FILES> file_handles;
    std::array<int, FW64_FILESYSTEM_MAX_OPEN_FILES> file_sizes;

    static std::unique_ptr<Filesystem> instance;
};

}
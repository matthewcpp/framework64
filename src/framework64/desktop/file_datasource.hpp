#pragma once

#include "framework64/media.h"

#include <filesystem>
#include <fstream>
#include <string>

namespace framework64 {

struct FileDataSource {
    FileDataSource();

    bool open(std::filesystem::path const & filesystem_path);
    void close();

    fw64DataSource interface;
    std::ifstream file;
    size_t file_size;

    static size_t sizeFunc(fw64DataSource* data_source);
    static size_t readFunc(fw64DataSource* data_source, void* buffer, size_t size, size_t count);
};

struct FileDataWriter {
    FileDataWriter();

    fw64DataWriter interface;
    std::ofstream file;

    bool open(std::filesystem::path const & filesystem_path);
    void close();

    static size_t writeFunc(fw64DataWriter* data_writer, const char* buffer, size_t size, size_t count);
};

}
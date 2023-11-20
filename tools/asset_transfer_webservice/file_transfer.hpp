#pragma once

#include "data_link/client.hpp"

#include <filesystem>
#include <fstream>
#include <vector>

namespace framework64::service {

class FileTransfer {
public:
    FileTransfer(framework64::data_link::Client& client) : client(client) {}

    bool execute(const std::filesystem::path& path);

private:
    void begin(const std::string& file_name, size_t file_size);
    void data(size_t file_size);

private:
    framework64::data_link::Client& client;
    std::ifstream source_file;
    std::vector<uint8_t> message_buffer;
};

}

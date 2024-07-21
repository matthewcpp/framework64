#include "file_transfer.hpp"

#include "fw64_file_downloader_message.h"

#include <cstring>
#include <iostream>
#include <fstream>

namespace framework64::service {

bool FileTransfer::execute(const std::filesystem::path& path) {
    if (!std::filesystem::is_regular_file(path)) {
        return false;
    }

    if (!client.isConnected()) {
        return false;
    }

    const auto file_name = path.filename().string();

    // TODO: trim filename if it doesnt fit in buffer?
    if (file_name.size() > FW64_FILE_DOWNLOADER_MAX_FILE_SIZE) {
        return false;
    }

    source_file.open(path, std::ios::binary);
    if (!source_file) {
        return false;
    }

    size_t file_size = static_cast<size_t>(std::filesystem::file_size(path));

    std::cout << "Transfering file: " << path << std::endl;

    begin(file_name, file_size);
    data(file_size);

    std::cout << "Transfer complete" << std::endl;

    return true;
}

struct BeginMessageHeader{
    uint32_t message_type;
    uint32_t file_name_size;
    uint32_t file_data_size;
};

void FileTransfer::begin(const std::string& file_name, size_t file_size) {
    message_buffer.resize(sizeof(BeginMessageHeader) + file_name.size());

    BeginMessageHeader header;
    header.message_type = client.littleToTargetEndian(Fw64_FILE_DOWNLOADER_MESSAGE_BEGIN);
    header.file_name_size = client.littleToTargetEndian(file_name.size());
    header.file_data_size = client.littleToTargetEndian(file_size);

    std::memcpy(message_buffer.data(), &header, sizeof(BeginMessageHeader));
    std::memcpy(message_buffer.data() + sizeof(BeginMessageHeader), file_name.data(), file_name.size());
    
    std::cout << "Sending header message: " << message_buffer.size() << " bytes" << std::endl; 
    client.sendMessage(message_buffer.data(), message_buffer.size());
}

struct DataMessageHeader {
    uint32_t message_type;
    uint32_t payload_size;
};

void FileTransfer::data(size_t file_size) {
    constexpr size_t MaxMessageSize = 4096;
    constexpr size_t max_payload_size = MaxMessageSize - sizeof(DataMessageHeader);

    DataMessageHeader header;
    header.message_type = client.littleToTargetEndian(Fw64_FILE_DOWNLOADER_MESSAGE_DATA);

    size_t data_remaining = file_size;
    while (data_remaining > 0) {
        const size_t payload_size = static_cast<uint32_t>(std::min(data_remaining, max_payload_size));
        header.payload_size = client.littleToTargetEndian(payload_size);

        message_buffer.resize(sizeof(DataMessageHeader) + payload_size);
        std::memcpy(message_buffer.data(), &header, sizeof(DataMessageHeader));
        source_file.read(reinterpret_cast<char*>(message_buffer.data() + sizeof(DataMessageHeader)) , payload_size);

        std::cout << "Sending data message: " << message_buffer.size() << " bytes" << std::endl; 
        client.sendMessage(message_buffer.data(), message_buffer.size());
        data_remaining -= payload_size;
    }
}

}

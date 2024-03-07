#include "framework64/desktop/save_file.hpp"

#include <cassert>
#include <iostream>
#include <limits>

#define N64_EEPROM_BLOCK_WRITE_TIME 0.016f

bool fw64SaveFile::init(std::string const& file_path, SaveFileType save_file_type) {
    file_type = save_file_type;

    if (file_type == SaveFileType::None) {
        enabled = false;
        return false;
    }

    auto file_size = getFileSize();

    if (file_size == 0) {
        std::cout << "Unknown Save File Type specified" << std::endl;
        return false;
    }

    file.open(file_path, std::fstream::in | std::fstream::out | std::ios::binary);

    if (!file){
        std::cout << "Creating Save File: " << file_path << std::endl;
        file.open(file_path, std::fstream::out | std::fstream::trunc | std::ios::binary); // create file if it doesnt exist
        file.close();
        file.open(file_path, std::fstream::in | std::fstream::out | std::ios::binary);
    }

    if (!file){
        std::cout << "Error opening Save file: " << file_path << std::endl;
        return false;
    }

    std::cout << "Loaded Save File: " << file_path << std::endl;

    enabled = true;

    // check to see if this is a new file
    file.seekg(0, std::ios::end);
    auto size = static_cast<uint32_t>(file.tellg());
    file.seekg(0);

    // file has already been initialized and is ready to go
    if (size == file_size) {
        return true;
    }

    // fill the eeprom with 0xFF's
    // https://n64squid.com/homebrew/libdragon/saving/eeprom/
    const auto byte = std::numeric_limits<uint8_t>::max();
    for (uint32_t i = 0; i < file_size; i++) {
        file.write(reinterpret_cast<const char*>(&byte), 1);
    }

    file.flush();

    return true;
}

void fw64SaveFile::update(float time_delta) {
    if (busy_time > 0.0f) {
        busy_time -= time_delta;
    }
}

uint32_t fw64SaveFile::getFileSize() const {
    switch(file_type) {
        case fw64SaveFile::SaveFileType::N64Eeprom4k:
            return 512;
        case fw64SaveFile::SaveFileType::N64Eeprom16K:
            return 2048;
        default:
            return 0;
    }
}

int fw64SaveFile::read(uint32_t base_addr, char* buffer, uint32_t buffer_size) {
    if (!isEnabled())
        return 1;

    file.seekg(base_addr);
    file.read(buffer, buffer_size);

    return 0;
}

int fw64SaveFile::write(uint32_t base_addr, const char* buffer, uint32_t buffer_size) {
    if (!isEnabled() || isBusy()) {
        return 1;
    }

    // Note: N64 EEPROM write is 16MS per 8 byte block
    assert(base_addr % 8 == 0);

    if (emulate_access_time) {
        busy_time = (buffer_size / 8) *  N64_EEPROM_BLOCK_WRITE_TIME;
    }

    file.seekp(base_addr);
    file.write(buffer, buffer_size);
    file.flush();

    return 0;
}

// C Interface
int fw64_save_file_valid(fw64SaveFile* save_file) {
    return save_file->isEnabled();
}

uint32_t fw64_save_file_size(fw64SaveFile* save_file) {
    return save_file->getFileSize();
}

int fw64_save_file_write(fw64SaveFile* save_file, uint32_t base_addr, void* buffer, uint32_t buffer_size) {
    assert(save_file->isEnabled());
    save_file->write(base_addr, reinterpret_cast<const char*>(buffer), buffer_size);

    return 0;
}

int fw64_save_file_read(fw64SaveFile* save_file, uint32_t base_addr, void* buffer, uint32_t buffer_size) {
    assert(save_file->isEnabled());
    save_file->read(base_addr, reinterpret_cast<char*>(buffer), buffer_size);

    return 0;
}

int fw64_save_file_busy(fw64SaveFile* save_file) {
    return save_file->isBusy();
}
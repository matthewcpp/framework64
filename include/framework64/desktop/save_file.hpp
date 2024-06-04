#pragma once

#include "framework64/save_file.h"

#include <array>
#include <fstream>
#include <string>

struct fw64SaveFile {
public:

enum class SaveFileType {
    None,
    N64Eeprom4k,
    N64Eeprom16K,
    Unknown
};

public:
    bool init(std::string const& file_path, SaveFileType save_file_type);

    int read(uint32_t base_addr, char* buffer, uint32_t buffer_size);
    int write(uint32_t base_addr, const char* buffer, uint32_t buffer_size);
    void update(float time_delta);

    [[nodiscard]] bool isEnabled() const { return enabled; }
    [[nodiscard]] bool isBusy() const { return busy_time > 0.0f; }
    [[nodiscard]] uint32_t getFileSize() const;

    void setEmulateAccessTime(bool should_emulate_access_time) { emulate_access_time = should_emulate_access_time; }

private:
    std::fstream file;
    SaveFileType file_type;
    float busy_time = 0.0f;
    bool enabled = false;
    bool emulate_access_time = true;
};
#pragma once

#include <filesystem>

#define UUID_SYSTEM_GENERATOR
#include <uuid.h>

namespace framework64::service {

class TemporaryDirectory {
public:
    TemporaryDirectory() {
        const auto temp_dir_name = uuids::uuid_system_generator{}();
        const auto temp_dir_path = std::filesystem::temp_directory_path() / uuids::to_string(temp_dir_name);

        if (std::filesystem::create_directory(temp_dir_path)) {
            _path = std::move(temp_dir_path);
        }
    }

    ~TemporaryDirectory() {
        if (!_path.empty()) {
            std::filesystem::remove_all(_path);
        }
    }

    inline const std::filesystem::path& path() const {
        return _path;
    }

private:
    std::filesystem::path _path;
};

}
#pragma once

#include "framework64/engine.h"
#include "framework64/desktop/save_file.h"
#include "framework64/desktop/shader_cache.h"
#include "framework64/desktop/n64_input_interface.h"

#include <memory>

namespace framework64 {
class Engine: public fw64Engine {
public:
    struct Settings {
        int screen_width = 320;
        int screen_height = 240;
        int data_link_port = 55662;
        std::string application_name = "framework64";
        std::string media_dir_name = "media";
        fw64SaveFile::SaveFileType save_file_type = fw64SaveFile::SaveFileType::N64Eeprom4k;
    };

public:
    Engine() = default;

    bool init(Settings const & settings);
    void update(float time_delta);

public:
    std::unique_ptr<ShaderCache> shader_cache;
    std::unique_ptr<N64InputInterface> n64_input_interface;

    std::string application_name;
};

}
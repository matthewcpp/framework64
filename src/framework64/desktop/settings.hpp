#pragma once

#include "framework64/desktop/save_file.hpp"
#include <string>

namespace framework64 {

struct Settings {
    int screen_width = 320;
    int screen_height = 240;
    int data_link_port = 55662;
    float display_scale = 3.0f;
    uint32_t fixed_update_rate = 30;
    uint32_t target_frame_rate = 60;
    std::string application_name = "framework64";
    std::string media_dir_name = "media";
    fw64SaveFile::SaveFileType save_file_type = fw64SaveFile::SaveFileType::N64Eeprom4k;
};

}

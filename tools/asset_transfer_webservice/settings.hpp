#pragma once

#include <string>

namespace framework64::service {

struct Settings {
    std::string framework64_dir;
    std::string node_path;
    int bind_port = 62187;

    std::string n64_rom_path;
    std::string desktop_websocket_uri = "ws://localhost:55662";
};

}

#pragma once

#include <string>

namespace framework64::service {

struct N64Settings {
    std::string rom_path;
};

struct DesktopSettings {
    std::string websocket_uri = "ws://localhost:55662";
};

struct Settings {
    std::string framework64_dir;
    std::string node_path;
    int webservice_port = 55660;
    
    DesktopSettings desktop;
    N64Settings n64;
};

}

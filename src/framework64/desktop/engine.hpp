#pragma once

#include "framework64/engine.h"

#include "framework64/desktop/display.hpp"
#include "framework64/desktop/save_file.hpp"
#include "framework64/desktop/shader_cache.hpp"
#include "framework64/desktop/n64_input_interface.hpp"

#include <memory>

namespace framework64 {
class Engine: public fw64Engine {
public:
    Engine() = default;

    bool init(Settings const & app_settings);
    void update(float time_delta);

public:
    std::unique_ptr<ShaderCache> shader_cache;
    std::unique_ptr<N64InputInterface> n64_input_interface;
    fw64Display display;
    Settings settings;

    std::string application_name;
};

}
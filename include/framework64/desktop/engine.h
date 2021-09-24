#pragma once

#include "framework64/engine.h"
#include "framework64/desktop/shader_cache.h"
#include "framework64/desktop/n64_input_interface.h"

#include <memory>

namespace framework64 {
class Engine: public fw64Engine {
public:
    Engine() = default;

    bool init(int screen_width, int screen_height);
    void update(float time_delta);

public:
    std::unique_ptr<ShaderCache> shader_cache;
    std::unique_ptr<N64InputInterface> n64_input_interface;
};

}
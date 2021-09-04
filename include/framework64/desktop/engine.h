#pragma once

#include "framework64/engine.h"
#include "framework64/desktop/shader_cache.h"

namespace framework64 {
class Engine: public fw64Engine {
public:
    Engine() = default;

    bool init(int screen_width, int screen_height);
    void update(float time_delta);

public:
    ShaderCache* shader_cache;
};

}
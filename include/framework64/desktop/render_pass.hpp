#pragma once

#include "framework64/render_pass.h"

#include "framework64/desktop/render_queue.hpp"

#include <array>
#include <cstring>
#include <vector>

struct fw64RenderPass {
public:
    fw64RenderPass(fw64Display* display, fw64Allocator* allocator);

    inline void setViewport(fw64Viewport const & vp) {
        viewport = vp;
    }

    inline void setViewMatrix(float const * view) {
        std::memcpy(view_matrix.data(), view, sizeof(float) * 16);
    }

    inline void setProjectionMatrix(float const * projection) {
        std::memcpy(projection_matrix.data(), projection, sizeof(float) * 16);
    }

    void begin();
    void end();

public:
    fw64Allocator* allocator;
    fw64Viewport viewport;
    std::array<float, 16> projection_matrix, view_matrix;

    fw64ClearFlags clear_flags = FW64_CLEAR_FLAG_NONE;
    std::array<float, 3> clear_color = {0.0f, 0.0f, 0.0f};
    float camera_near = 0, camera_far = 1;

    framework64::RenderQueue render_queue;

    std::array<float, 3> fog_color = {0.6f, 0.6f, 0.6f};
    float fog_begin = 0.5;
    float fog_end = 1.0f;
    bool fog_enabled = 0;
    bool depth_testing_enabled = true;
};
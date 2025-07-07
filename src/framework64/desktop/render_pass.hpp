#pragma once

#include "framework64/render_queue.h"
#include "framework64/render_pass.h"
#include "framework64/renderer.h"

#include <array>
#include <cstring>
#include <vector>

struct Light {
    std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f};
    std::array<float, 4> direction = {0.57735f, -0.57735f, 0.57735f, 1.0f};
};

struct LightInfo {
    Light light;
    int active = 0;
};

struct LightingInfo {
    std::array<LightInfo, FW64_RENDERER_MAX_LIGHT_COUNT> lights;
    std::array<float, 4> ambient_color = {0.1f, 0.1f, 0.1f, 1.0f};
};

struct fw64RenderPass {
public:
    fw64RenderPass(fw64Display* display, fw64Allocator* allocator);
    ~fw64RenderPass();

    inline void setViewport(fw64Viewport const & vp) {
        viewport = vp;
    }

    inline void setViewMatrix(float const * view) {
        std::memcpy(view_matrix.data(), view, sizeof(float) * 16);
    }

    inline void setProjectionMatrix(float const * projection) {
        std::memcpy(projection_matrix.data(), projection, sizeof(float) * 16);
    }

public:
    void setAmbientLightColor(uint8_t r, uint8_t g, uint8_t b);
    void setLightEnabled(int index, int enabled);
    void setLightDirection(int index, float x, float y, float z);
    void setLightColor(int index, uint8_t r, uint8_t g, uint8_t b);

    void begin();
    void end();

public:
    fw64Allocator* allocator;
    fw64Viewport viewport;
    std::array<float, 16> projection_matrix, view_matrix;

    fw64PrimitiveMode primitive_mode = FW64_PRIMITIVE_MODE_TRIANGLES;
    fw64ClearFlags clear_flags = FW64_CLEAR_FLAG_NONE;
    std::array<float, 3> clear_color = {0.0f, 0.0f, 0.0f};
    float camera_near = 0, camera_far = 1;

    fw64RenderQueue render_queue;
    LightingInfo lighting_info;

    std::array<float, 3> fog_color = {0.6f, 0.6f, 0.6f};
    float fog_begin = 0.5;
    float fog_end = 1.0f;
    bool fog_enabled = 0;
    bool depth_testing_enabled = true;
};
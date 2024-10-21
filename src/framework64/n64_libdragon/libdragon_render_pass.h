#pragma once

#include "framework64/render_pass.h"
#include "framework64/render_queue.h"
#include "framework64/renderer.h"

typedef struct {
    float color[4]; // = {1.0f, 1.0f, 1.0f, 1.0f};
    Vec3 direction; // = {0.57735f, -0.57735f, 0.57735f, 1.0f};
    int active; // = 0;
} Light;

typedef struct {
    Light lights[FW64_RENDERER_MAX_LIGHT_COUNT];
    float ambient_color[4]; // = {0.1f, 0.1f, 0.1f, 1.0f};
} LightingInfo;

void fw64_libdragon_lighting_info_init(LightingInfo* lighting_info);
void fw64_libdragon_lighting_info_apply(LightingInfo* lighting_info);

typedef enum {
    FW64_LIBDRAGON_RENDERPASS_FEATURE_NONE = 0,
    FW64_LIBDRAGON_RENDERPASS_FEATURE_DEPTH_TESTING = 1 << 0
} fw64LibdragonRenderFeatures;

struct fw64RenderPass{
    fw64Allocator* allocator;
    fw64Viewport viewport;
    float projection_matrix[16], view_matrix[16];
    fw64LibdragonRenderFeatures render_features;
    fw64RenderQueue render_queue;
    LightingInfo lighting_info;
};

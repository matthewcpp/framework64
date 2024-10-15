#pragma once

#include "framework64/render_pass.h"
#include "framework64/render_queue.h"

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
};

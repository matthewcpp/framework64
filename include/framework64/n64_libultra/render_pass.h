#pragma once

#include "framework64/render_pass.h"

#include "framework64/n64_libultra/render_queue.h"

#include <nusys.h>

#include <stdint.h>

typedef enum {
    N64_RENDERER_FEATURE_NONE = 0,
    N64_RENDERER_FEATURE_AA = 1 << 0,
    N64_RENDERER_FEATURE_DEPTH_TEST = 1 << 1,
    N64_RENDERER_FEATURE_FOG = 1 << 2,
    N64_RENDERER_FEATURE_COLOR_INDEX_MODE = 1 << 3,
    N64_RENDERER_FEATURE_SPRITE_SCISSOR = 1 << 4
} fw64N64RendererFeature;

#define SET_RENDERER_FEATURE(renderer, feature) ((renderer)->enabled_features |= (feature))
#define UNSET_RENDERER_FEATURE(renderer, feature) ((renderer)->enabled_features &= ~(feature))
#define GET_RENDERER_FEATURE(renderer, feature) ((renderer)->enabled_features & (feature))

struct fw64RenderPass {
    fw64Viewport viewport;
    Mtx projection_matrix;
    Mtx view_matrix;
    fw64Allocator* allocator;
    uint16_t persp_norm;
    uint16_t padding;

    fw64N64RenderQueue render_queue;
    fw64ClearFlags clear_flags;
    fw64N64RendererFeature enabled_features;
    u16 clear_color;
};

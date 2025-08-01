#pragma once

#include "framework64/render_pass.h"
#include "framework64/renderer.h"

#include "framework64/render_queue.h"

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

typedef struct {
    Light lights[FW64_RENDERER_MAX_LIGHT_COUNT];
    Ambient ambient;
    uint16_t active_count;
} LightingInfo;

#define SET_RENDERER_FEATURE(renderer, feature) ((renderer)->enabled_features |= (feature))
#define UNSET_RENDERER_FEATURE(renderer, feature) ((renderer)->enabled_features &= ~(feature))
#define GET_RENDERER_FEATURE(renderer, feature) ((renderer)->enabled_features & (feature))

struct fw64RenderPass {
    Vp n64_viewport;
    Mtx projection_matrix;
    Mtx view_matrix;

    fw64Viewport viewport;
    LightingInfo lighting_info;
    fw64Allocator* allocator;
    uint16_t persp_norm;
    uint16_t padding;

    fw64RenderQueue render_queue;
    fw64ClearFlags clear_flags;
    fw64N64RendererFeature enabled_features;
    fw64PrimitiveMode primitive_mode;
    u16 clear_color;
    fw64ColorRGBA8 fog_color;
    s32 fog_min, fog_max;
};

#define fw64_n64_lighting_info_light_is_enabled(lighting_info, index) ((lighting_info)->lights[(index)].l.pad3)
#define fw64_n64_lighting_info_set_enabled_flag(lighting_info, index, enabled) ((lighting_info)->lights[(index)].l.pad3 = (char)(enabled))
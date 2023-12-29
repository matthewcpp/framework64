#pragma once

#include "framework64/material.h"

#include "framework64/color.h"
#include "framework64/texture.h"

#include <stdint.h>

#define FW64_MATERIAL_NO_TEXTURE UINT32_MAX

struct fw64Material {
    fw64ColorRGBA8 color;
    fw64Texture* texture;
    uint32_t texture_frame;
    uint32_t shading_mode;
};

void fw64_n64_material_init(fw64Material* material);

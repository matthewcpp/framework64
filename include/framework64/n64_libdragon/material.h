#pragma once

#include "framework64/n64_libdragon/texture.h"

#include "framework64/material.h"
#include "framework64/color.h"

#define FW64_MATERIAL_NO_TEXTURE UINT32_MAX

struct fw64Material {
    uint32_t shading_mode;
    fw64Texture* texture;
    float color[4];
};
#pragma once

#include "framework64/material.h"

struct fw64Material {
    uint32_t shading_mode;
    fw64Texture* texture;
    fw64ColorRGBA8 color;
};

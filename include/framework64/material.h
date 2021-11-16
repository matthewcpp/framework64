#pragma once

/** \file material.h */

#include "framework64/texture.h"

typedef struct fw64Material fw64Material;

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FW64_SHADING_MODE_UNSET,
    FW64_SHADING_MODE_UNLIT_VERTEX_COLORS,
    FW64_SHADING_MODE_UNLIT_TEXTURED,
    FW64_SHADING_MODE_GOURAUD,
    FW64_SHADING_MODE_GOURAUD_TEXTURED,
    FW64_SHADING_MODE_SPRITE,
    FW64_SHADING_MODE_DECAL_TEXTURE,
    FW64_SHADING_MODE_FILL,
    Fw64_SHADING_MODE_COUNT
} fw64ShadingMode;

fw64Texture* fw64_material_get_texture(fw64Material* material);
void fw64_material_set_texture_frame(fw64Material* material, int frame);
int fw64_material_get_texture_frame(fw64Material* material);
void fw64_material_set_shading_mode(fw64Material* material, fw64ShadingMode mode);

#ifdef __cplusplus
}
#endif

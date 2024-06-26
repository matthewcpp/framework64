#pragma once

/** \file material.h */

#include "framework64/color.h"
#include "framework64/texture.h"

typedef struct fw64Material fw64Material;

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FW64_SHADING_MODE_UNSET,

    /** Vertex color * material color */
    FW64_SHADING_MODE_VERTEX_COLOR,

    /** Vertex color * material color */
    FW64_SHADING_MODE_VERTEX_COLOR_TEXTURED,

    /** Gouraud Shaded: Untextured */
    FW64_SHADING_MODE_GOURAUD,

    /** Gouraud Shaded: Textured */
    FW64_SHADING_MODE_GOURAUD_TEXTURED,

    /** Vertex color * Texture color */
    FW64_SHADING_MODE_UNLIT_TEXTURED,

    FW64_SHADING_MODE_DECAL_TEXTURE,
    FW64_SHADING_MODE_SPRITE,
    FW64_SHADING_MODE_FILL,
    FW64_SHADING_MODE_LINE,

    FW64_SHADING_MODE_COUNT
} fw64ShadingMode;

void fw64_material_set_texture(fw64Material* material, fw64Texture* texture, uint32_t initial_frame);
fw64Texture* fw64_material_get_texture(fw64Material* material);

/**
 * Sets the active texture frame
 * Note that if the image was loaded with DMA mode on N64 this will immediately cause a DMA read.
 */
void fw64_material_set_texture_frame(fw64Material* material, uint32_t frame);
uint32_t fw64_material_get_texture_frame(fw64Material* material);

void fw64_material_set_shading_mode(fw64Material* material, fw64ShadingMode mode);
void fw64_material_set_color(fw64Material* material, uint8_t r, uint8_t g, uint8_t b);
void fw64_material_get_color(fw64Material* material, fw64ColorRGBA8* color);

#ifdef __cplusplus
}
#endif

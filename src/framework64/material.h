#pragma once

/** \file material.h */

#include "framework64/color.h"
#include "framework64/texture.h"

typedef struct fw64Material fw64Material;

#ifdef __cplusplus
extern "C" {
#endif

// This needs to be kept in sync with pipeline/gltf/material.js
typedef enum {
    FW64_SHADING_MODE_UNSET,

    /** Vertex color * material color */
    FW64_SHADING_MODE_UNLIT = 1,

    /** Vertex color * material color */
    FW64_SHADING_MODE_UNLIT_TEXTURED = 2,

    /** Gouraud Shaded: Untextured */
    FW64_SHADING_MODE_LIT = 3,

    /** Gouraud Shaded: Textured */
    FW64_SHADING_MODE_LIT_TEXTURED = 4,

    FW64_SHADING_MODE_DECAL_TEXTURE = 5,

    FW64_SHADING_MODE_LINE = 6,

    /** Currently only used for N64...THis will go away with transparency support */
    FW64_SHADING_MODE_UNLIT_TRANSPARENT_TEXTURED = 7,

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
fw64ShadingMode fw64_material_get_shading_mode(const fw64Material* material);

void fw64_material_set_color(fw64Material* material, fw64ColorRGBA8 color);
fw64ColorRGBA8 fw64_material_get_color(fw64Material* material);

/** Returns a duplicate of the source material */
fw64Material* fw64_material_clone(const fw64Material* material, fw64Allocator* allocator);

#ifdef __cplusplus
}
#endif

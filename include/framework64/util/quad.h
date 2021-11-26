#pragma once

/** \file quad.h */

#include "framework64/allocator.h"
#include "framework64/color.h"
#include "framework64/engine.h"
#include "framework64/mesh.h"

#include <stdint.h>

typedef struct {
    fw64Image* image;
    int image_asset_index;
    int is_animated;
    float min_s;
    float max_s;
    float min_t;
    float max_t;
} fw64TexturedQuadParams;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_textured_quad_params_init(fw64TexturedQuadParams* params);

fw64Mesh* fw64_textured_quad_create(fw64Engine* engine, int image_asset_index, fw64Allocator* allocator);
fw64Mesh* fw64_textured_quad_create_with_params(fw64Engine* engine, fw64TexturedQuadParams* params, fw64Allocator* allocator);
#ifdef __cplusplus
}
#endif

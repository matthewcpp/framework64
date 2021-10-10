#pragma once

/** \file quad.h */

#include "framework64/color.h"
#include "framework64/engine.h"
#include "framework64/mesh.h"


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

fw64Mesh* textured_quad_create(fw64Engine* engine, int image_asset_index);
fw64Mesh* textured_quad_create_with_params(fw64Engine* engine, int image_asset_index, float max_s, float max_t);
fw64Mesh* textured_quad_create_with_image(fw64Engine* engine, fw64Image* image, int frame_index);

#ifdef __cplusplus
}
#endif

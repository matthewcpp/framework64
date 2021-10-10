#pragma once

/** \file material.h */

#include "framework64/texture.h"

typedef struct fw64Material fw64Material;

#ifdef __cplusplus
extern "C" {
#endif

fw64Texture* fw64_material_get_texture(fw64Material* material);
void fw64_material_set_texture_frame(fw64Material* material, int frame);
int fw64_material_get_texture_frame(fw64Material* material);

#ifdef __cplusplus
}
#endif

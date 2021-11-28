#include "framework64/util/quad.h"

#include <string.h>

void fw64_textured_quad_params_init(fw64TexturedQuadParams* params) {
    params->image = NULL;
    params->image_asset_index = -1;
    params->is_animated = 0;
    params->min_s = 0.0f;
    params->max_s = 1.0f;
    params->min_t = 0.0f;
    params->max_t = 1.0f;
}
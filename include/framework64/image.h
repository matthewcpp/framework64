#pragma once

#include "framework64/asset_database.h"

#include <stdint.h>

typedef struct fw64Image fw64Image;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_image_reference_add(fw64Image* image);
void fw64_image_reference_remove(fw64Image* image);

fw64Image* fw64_image_load(fw64AssetDatabase* asset_database, uint32_t index);

#ifdef __cplusplus
}
#endif
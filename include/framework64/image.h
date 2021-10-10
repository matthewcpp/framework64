#pragma once

#include "framework64/asset_database.h"

#include <stdint.h>

typedef struct fw64Image fw64Image;

#ifdef __cplusplus
extern "C" {
#endif

/** Loads an image from the asset database.  
 * You will need to call \ref fw64_image_delete to free resources for this image
 **/
fw64Image* fw64_image_load(fw64AssetDatabase* asset_database, uint32_t index);

/**
 * Deletes the image and frees all assocated resources for an image created with \ref fw64_image_load
 **/
void fw64_image_delete(fw64Image* image);

#ifdef __cplusplus
}
#endif
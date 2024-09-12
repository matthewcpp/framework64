#pragma once

/** \file image.h */

#include "framework64/allocator.h"
#include "framework64/data_io.h"

#include "framework64/asset_database.h"

#include <stdint.h>

typedef struct fw64Image fw64Image;

#ifdef __cplusplus
extern "C" {
#endif

/** loads an image from a generic datasource. */
fw64Image* fw64_image_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator);

/**
 * Deletes the image and frees all assocated resources for an image created with \ref fw64_image_load
 **/
void fw64_image_delete(fw64AssetDatabase* asset_database, fw64Image* image, fw64Allocator* allocator);

/**
 * Loads the specified frame into from ROM.
 * This method is a no-op if the image was not loaded in DMA mode.
 */
void fw64_image_load_frame(fw64Image* image, uint32_t frame);

/**
 * Gets the number of indexed color palettes this image has.
 * All index colored images will have at least 1 color palette
*/
uint16_t fw64_image_get_palette_count(fw64Image* image);

#ifdef __cplusplus
}
#endif

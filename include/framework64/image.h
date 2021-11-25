#pragma once

/** \file image.h */

#include "framework64/allocator.h"
#include "framework64/asset_database.h"

#include <stdint.h>

typedef struct fw64Image fw64Image;

typedef enum {
    /**
     * The default mode for images.
     * In this case the entire image will be loaded into memory.
     * Useful if you need to draw multiple frames of the image in a single render pass
     * */
    FW64_IMAGE_FLAG_NONE = 0,

    /**
     * Loads only a single frame of the image at a time.
     * This mode will save memory but you will not be able to draw multiple parts of the image in a single render pass.
     * This mode is ignored on non N64 platforms.
     */
    FW64_IMAGE_FLAG_DMA_MODE = 1
} fw64ImageFlag;

#ifdef __cplusplus
extern "C" {
#endif

/** Loads an image from the asset database.
 * You will need to call \ref fw64_image_delete to free resources for this image
 **/
fw64Image* fw64_image_load(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator);

/** Loads an image from the asset database with options.
 * You will need to call \ref fw64_image_delete to free resources for this image
 **/
fw64Image* fw64_image_load_with_options(fw64AssetDatabase* asset_database, uint32_t asset_index, uint32_t options, fw64Allocator* allocator);

/**
 * Deletes the image and frees all assocated resources for an image created with \ref fw64_image_load
 **/
void fw64_image_delete(fw64AssetDatabase* asset_database, fw64Image* image, fw64Allocator* allocator);

#ifdef __cplusplus
}
#endif
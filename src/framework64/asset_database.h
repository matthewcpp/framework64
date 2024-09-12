#pragma once

/** \file asset_database.h */

#include "framework64/allocator.h"
#include "framework64/animation_data.h"
#include "framework64/data_io.h"
#include "framework64/skinned_mesh.h"

#include <stdint.h>

typedef struct fw64Image fw64Image;
typedef struct fw64Font fw64Font;
typedef struct fw64Mesh fw64Mesh;
typedef struct fw64Scene fw64Scene;

typedef uint64_t fw64AssetId;

#define FW64_INVALID_ASSET_ID UINT64_MAX

typedef struct fw64AssetDatabase fw64AssetDatabase;

#define FW64_ASSETS_MAX_OPEN_DATASOURCES 4


#ifdef __cplusplus
extern "C" {
#endif

// TODO: this should not be void* change this after data source loading is all setup

/** 
 * Loads an image from the asset bundle given a handle.  
 * Note this method does not support DMA mode images, instead use \ref fw64_assets_load_image_dma */
fw64Image* fw64_assets_load_image(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator);

/**
 * Loads an image in DMA mode from the asset bundle.
 * Images in DMA mode load only a single frame of the image at a time.
 * This mode will save memory but you will not be able to draw multiple parts of the image in a single render pass.
 * Note that DMA mode images do not currently support color indexed images.
 * This mode is ignored on non N64 platforms.
*/
fw64Image* fw64_assets_load_image_dma(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator);

/** Loads a font with the given index from the asset bundle. */
fw64Font* fw64_assets_load_font(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator);

/** Loads a mesh from the asset bundle. */
fw64Mesh* fw64_assets_load_mesh(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator);

fw64Scene* fw64_assets_load_scene(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator);

fw64AnimationData* fw64_assets_load_animation_data(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator);

fw64SkinnedMesh* fw64_assets_load_skinned_mesh(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator);

/** 
 * Opens a datasource for an asset with the supplied asset_id or NULL if opening failed.
 * Note the pointer returned from this function is managed by the library and should not be freed by the user.
 * When you no longer need the datasource call \ref fw64_assets_close_datasource
 */
fw64DataSource* fw64_assets_open_datasource(fw64AssetDatabase* asset_database, fw64AssetId asset_id);
void fw64_assets_close_datasource(fw64AssetDatabase* asset_database, fw64DataSource* datasource);

#ifdef __cplusplus
}
#endif

#pragma once

/** \file asset_database.h */

#include "framework64/asset_database.h"
#include "framework64/texture.h"

#ifdef __cplusplus
extern "C" {
#endif

fw64Texture* fw64_texture_util_create_from_loaded_image(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator);
void fw64_texture_util_delete_tex_and_image(fw64Texture* texture, fw64AssetDatabase* asset_database, fw64Allocator* allocator);

#ifdef __cplusplus
}
#endif

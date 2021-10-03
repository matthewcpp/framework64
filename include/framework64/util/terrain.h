#pragma once

#include "framework64/asset_database.h"
#include "framework64/mesh.h"
#include "framework64/renderer.h"
#include "framework64/transform.h"

#include <stdint.h>

typedef struct fw64Terrain fw64Terrain;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_terrain_draw(fw64Terrain* terrain, fw64Renderer* renderer);
float fw64_terrain_get_height(fw64Terrain* terrain, float x, float z);

uint32_t fw64_terrain_get_mesh_count(fw64Terrain* terrain);
fw64Mesh* fw64_terrain_get_mesh(fw64Terrain* terrain, uint32_t index);
fw64Transform* fw64_terrain_get_transform(fw64Terrain* terrain);

fw64Terrain* fw64_terrain_load(fw64AssetDatabase* database, uint32_t index);
void fw64_terrain_delete(fw64Terrain* terrain);

#ifdef __cplusplus
}
#endif
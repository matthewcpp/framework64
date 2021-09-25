#pragma once

#include "framework64/asset_database.h"
#include "framework64/mesh.h"
#include "framework64/renderer.h"
#include "framework64/transform.h"

#include <stdint.h>

typedef struct {
    fw64Transform transform;
    fw64Mesh** meshes;
    uint32_t mesh_count;
} fw64Terrain;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_terrain_draw(fw64Terrain* terrain, fw64Renderer* renderer);
float fw64_terrain_get_height(fw64Terrain* terrain, float x, float z);

fw64Terrain* fw64_terrain_load(fw64AssetDatabase* database, uint32_t index);
void fw64_terrain_delete(fw64Terrain* terrain);

#ifdef __cplusplus
}
#endif
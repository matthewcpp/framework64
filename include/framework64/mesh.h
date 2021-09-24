#pragma once

#include "framework64/asset_database.h"
#include "framework64/box.h"
#include "framework64/material.h"
#include "framework64/texture.h"

#include <stdint.h>

typedef enum {
    FW64_SHADING_MODE_UNSET,
    FW64_SHADING_MODE_UNLIT_VERTEX_COLORS,
    FW64_SHADING_MODE_UNLIT_TEXTURED,
    FW64_SHADING_MODE_GOURAUD,
    FW64_SHADING_MODE_GOURAUD_TEXTURED,
    FW64_SHADING_MODE_SPRITE,
    FW64_SHADING_MODE_FILL
} fw64ShadingMode;


typedef struct fw64Primitive fw64Primitive;
typedef struct fw64Mesh fw64Mesh;

#ifdef __cplusplus
extern "C" {
#endif

/**
Loads a mesh and all dependant assets from the asset bundle.
@param asset_index the mesh asset index.  Refer to the generated assets.h file.
*/
fw64Mesh* fw64_mesh_load(fw64AssetDatabase* assets, uint32_t index);

/**
Cleans up a mesh that was manually constructed after calling \ref mesh_init
*/
void fw64_mesh_delete(fw64Mesh* mesh);

void fw64_mesh_get_bounding_box(fw64Mesh* mesh, Box* box);
int fw64_mesh_get_primitive_count(fw64Mesh* mesh);

int fw64_mesh_get_texture_count(fw64Mesh* mesh);
fw64Texture* fw64_mesh_get_texture(fw64Mesh* mesh, int index);

fw64Primitive* fw64_mesh_get_primitive(fw64Mesh* mesh, int index);
fw64Material* fw64_mesh_primitive_get_material(fw64Primitive* primitive);

#ifdef __cplusplus
}
#endif

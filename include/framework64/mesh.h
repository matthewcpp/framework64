#pragma once

/** \file mesh.h */

#include "framework64/allocator.h"
#include "framework64/asset_database.h"
#include "framework64/box.h"
#include "framework64/material.h"
#include "framework64/texture.h"

#include <limits.h>
#include <stdint.h>

#define FW64_JOINT_INDEX_NONE UINT32_MAX

typedef struct fw64Mesh fw64Mesh;
typedef struct fw64Primitive fw64Primitive;

#ifdef __cplusplus
extern "C" {
#endif

/**
Loads a mesh and all dependant assets from the asset bundle.
@param asset_index the mesh asset index.  Refer to the generated assets.h file.
*/
fw64Mesh* fw64_mesh_load(fw64AssetDatabase* assets, uint32_t index, fw64Allocator* allocator);

/**
Cleans up a mesh that was manually constructed after calling \ref mesh_init
*/
void fw64_mesh_delete(fw64AssetDatabase* assets, fw64Mesh* mesh, fw64Allocator* allocator);

void fw64_mesh_get_bounding_box(fw64Mesh* mesh, Box* box);
int fw64_mesh_get_primitive_count(fw64Mesh* mesh);

Box fw64_mesh_get_bounding_for_primitive(fw64Mesh* mesh, int index);
fw64Material* fw64_mesh_get_material_for_primitive(fw64Mesh* mesh, int index);

#ifdef __cplusplus
}
#endif

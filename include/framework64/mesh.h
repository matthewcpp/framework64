#pragma once

/** \file mesh.h */

#include "framework64/allocator.h"
#include "framework64/box.h"
#include "framework64/data_io.h"
#include "framework64/material.h"
#include "framework64/texture.h"

#include <limits.h>
#include <stdint.h>

#define FW64_JOINT_INDEX_NONE UINT32_MAX

typedef enum {
    FW64_PRIMITIVE_MODE_UNSET,
    FW64_PRIMITIVE_MODE_TRIANGLES,
    FW64_PRIMITIVE_MODE_LINES
} fw64PrimitiveMode;

typedef enum {
    FW64_VERTEX_ATTRIBUTE_UNSPECIFIED = 0,
    FW64_VERTEX_ATTRIBUTE_POSITION = (1 << 0),
    FW64_VERTEX_ATTRIBUTE_NORMAL = (1 << 1),
    FW64_VERTEX_ATTRIBUTE_TEXCOORD = (1 << 2),
    FW64_VERTEX_ATTRIBUTE_COLOR = (1 << 3)
} fw64VertexAttributes;

typedef struct fw64AssetDatabase fw64AssetDatabase;
typedef struct fw64MaterialBundle fw64MaterialBundle;
typedef struct fw64Mesh fw64Mesh;
typedef struct fw64Primitive fw64Primitive;

#ifdef __cplusplus
extern "C" {
#endif

fw64Mesh* fw64_mesh_load_from_datasource(fw64AssetDatabase* assets, fw64DataSource* data_source, fw64Allocator* allocator);
fw64Mesh* fw64_mesh_load_from_datasource_with_bundle(fw64AssetDatabase* assets, fw64DataSource* data_source, fw64MaterialBundle* material_bundle, fw64Allocator* allocator);

/** Cleans up a mesh that was manually constructed after calling \ref mesh_init */
void fw64_mesh_delete(fw64Mesh* mesh, fw64AssetDatabase* assets, fw64Allocator* allocator);

Box fw64_mesh_get_bounding_box(fw64Mesh* mesh);
int fw64_mesh_get_primitive_count(fw64Mesh* mesh);

fw64Material* fw64_mesh_get_material_for_primitive(fw64Mesh* mesh, uint32_t index);

#ifdef __cplusplus
}
#endif

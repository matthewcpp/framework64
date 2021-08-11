#ifndef FW64_MESH_H
#define FW64_MESH_H

#include "framework64/box.h"
#include "framework64/texture.h"

#include <nusys.h>
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

#define FW64_MATERIAL_NO_TEXTURE UINT16_MAX
#define FW64_MATERIAL_NO_COLOR UINT32_MAX

typedef struct {
    uint32_t color;
    uint16_t texture;
    uint16_t texture_frame;
    uint32_t mode;
} fw64Material;

typedef struct {
    Box bounding_box;
    uint32_t vertices; // offset into mesh vertex array
    uint32_t display_list; // offset into mesh display_list array
    fw64Material material;
} fw64Primitive;

typedef struct {
    uint32_t primitive_count;
    uint32_t color_count;
    uint32_t texture_count;
    uint32_t vertex_count;
    uint32_t display_list_count;
    uint32_t _vertex_pointer_data_size; // only used during loading
    Box bounding_box;
} fw64MeshInfo;

typedef struct {
    fw64MeshInfo info;
    fw64Primitive* primitives;
    fw64Texture* textures;
    Lights1* colors;
    Vtx* vertex_buffer;
    Gfx* display_list;
} fw64Mesh;

/**
Loads a mesh and all dependant assets from the asset bundle.
@param asset_index the mesh asset index.  Refer to the generated assets.h file.
*/
int fw64_mesh_load(int asset_index, fw64Mesh* mesh);

/**
Unloads a mesh that was loaded with \ref mesh_load
*/
void fw64_mesh_unload(fw64Mesh* mesh);

/**
Initializes a mesh for manual construction.
*/
void fw64_mesh_init(fw64Mesh* mesh);

/**
Cleans up a mesh that was manually constructed after calling \ref mesh_init
*/
void fw64_mesh_uninit(fw64Mesh* mesh);

#endif

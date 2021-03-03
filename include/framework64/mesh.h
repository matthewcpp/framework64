#ifndef FW64_MESH_H
#define FW64_MESH_H

#include "framework64/box.h"
#include "framework64/sprite.h"

#include <nusys.h>
#include <stdint.h>

typedef enum {
    SHADING_MODE_UNSET,
    SHADING_MODE_UNLIT_VERTEX_COLORS,
    SHADING_MODE_UNLIT_TEXTURED,
    SHADING_MODE_GOURAUD,
    SHADING_MODE_GOURAUD_TEXTURED,
    SHADING_MODE_SPRITE,
    SHADING_MODE_FILL
} ShadingMode;

#define MATERIAL_NO_TEXTURE UINT16_MAX
#define MATERIAL_NO_COLOR UINT32_MAX

typedef struct {
    uint32_t color;
    uint16_t texture;
    uint16_t texture_frame;
    uint32_t mode;
} Material;

typedef struct {
    Box bounding_box;
    uint32_t vertices; // offset into mesh vertex array
    uint32_t display_list; // offset into mesh display_list array
    Material material;
} Primitive;

typedef struct {
    uint32_t primitive_count;
    uint32_t color_count;
    uint32_t texture_count;
    uint32_t vertex_count;
    uint32_t display_list_count;
    uint32_t _vertex_pointer_data_size; // only used during loading
    Box bounding_box;
} MeshInfo;

typedef struct {
    MeshInfo info;
    Primitive* primitives;
    ImageSprite* textures;
    Lights1* colors;
    Vtx* vertex_buffer;
    Gfx* display_list;
} Mesh;

/**
Loads a mesh and all dependant assets from the asset bundle.
@param asset_index the mesh asset index.  Refer to the generated assets.h file.
*/
int mesh_load(int asset_index, Mesh* mesh);

/**
Unloads a mesh that was loaded with \ref mesh_load
*/
void mesh_unload(Mesh* mesh);

/**
Initializes a mesh for manual construction.
*/
void mesh_init(Mesh* mesh);

/**
Cleans up a mesh that was manually constructed after calling \ref mesh_init
*/
void mesh_uninit(Mesh* mesh);

#endif
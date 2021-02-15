#ifndef FW64_MESH_H
#define FW64_MESH_H

#include "framework64/box.h"

#include <nusys.h>
#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t* data;
} Texture;

typedef enum {
    SHADING_MODE_UNSET,
    SHADING_MODE_UNLIT,
    SHADING_MODE_UNLIT_TEXTURED,
    SHADING_MODE_GOURAUD,
    SHADING_MODE_GOURAUD_TEXTURED,
    SHADING_MODE_SPRITE,
    SHADING_MODE_FILL
} ShadingMode;

#define MATERIAL_NO_TEXTURE UINT32_MAX

typedef struct {
    uint32_t color;
    uint32_t texture;
    uint32_t mode;
} Material;

typedef struct {
    Box bounding_box;
    uint32_t vertices;
    uint32_t display_list;
    Material material;
} Primitive;

typedef struct {
    uint32_t primitive_count;
    uint32_t color_count;
    uint32_t texture_count;
    uint32_t vertex_count;
    uint32_t display_list_count;
    uint32_t _vertex_pointer_data_size; // only used during loading
} MeshInfo;

typedef struct {
    MeshInfo info;

    Primitive* primitives;
    Texture* textures;
    Lights1* colors;
    Vtx* vertex_buffer;
    Gfx* display_list;
} Mesh;

int mesh_load(int asset_index, Mesh* mesh);
void mesh_unload(Mesh* mesh);

void mesh_compute_bounding_box(Mesh* mesh, Box* box);

#endif
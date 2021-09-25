#pragma once

#include "framework64/mesh.h"

#include "framework64/box.h"
#include "framework64/n64/texture.h"

#include <nusys.h>

#include <stdint.h>

#define FW64_MATERIAL_NO_TEXTURE UINT32_MAX
#define FW64_MATERIAL_NO_COLOR UINT32_MAX

struct fw64Material {
    uint32_t color;
    fw64Texture* texture;
    uint32_t texture_frame;
    uint32_t mode;
};

struct fw64Primitive{
    Box bounding_box;
    uint32_t vertices; // offset into mesh vertex array
    uint32_t display_list; // offset into mesh display_list array
    fw64Material material;
} ;

typedef struct {
    uint32_t primitive_count;
    uint32_t color_count;
    uint32_t texture_count;
    uint32_t vertex_count;
    uint32_t display_list_count;
    uint32_t _vertex_pointer_data_size; // only used during loading
    Box bounding_box;
} fw64MeshInfo;

struct fw64Mesh {
    fw64MeshInfo info;
    fw64Primitive* primitives;
    fw64Texture* textures;
    Lights1* colors;
    Vtx* vertex_buffer;  //contains ALL the vertices for this mesh
    Gfx* display_list;  //contains ALL the display lists for this mesh
};

void fw64_n64_mesh_init(fw64Mesh* mesh);

/**
Unloads a mesh that was loaded with \ref mesh_load
*/
void fw64_n64_mesh_unload(fw64Mesh* mesh);

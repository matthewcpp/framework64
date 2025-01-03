#pragma once

#include "framework64/mesh.h"

#include "framework64/box.h"
#include "framework64/color.h"
#include "framework64/n64_libultra/material_bundle.h"

#include <nusys.h>

#include <stdint.h>

#define FW64_PRIMITIVE_NO_MATERIAL UINT32_MAX

/** This should correspond to PrimitiveInfo in pipeline/MeshWriter.js  */
struct fw64Primitive {
    Vtx* vertices; // offset into mesh vertex array
    Gfx* display_list; // offset into mesh display_list array
    uint32_t joint_index; // used for skinning
};

#define FW64_MESH_FLAGS_IMAGES_ARE_SHARED 1

/// this needs to line up with pipeline/n64_libultra/MeshWriter.js
typedef struct {
    uint16_t primitive_count;
    uint16_t vertex_count;
    uint16_t display_list_count;
    uint16_t unused;
    uint16_t _vertex_pointer_data_size; // only used during loading
    uint16_t _material_bundle_count;
    Box bounding_box;
} fw64MeshInfo;

struct fw64Mesh {
    fw64MeshInfo info;
    fw64Primitive* primitives;
    fw64MaterialBundle* material_bundle;
    Vtx* vertex_buffer;  //contains ALL the vertices for this mesh
    Gfx* display_list;  //contains ALL the display lists for this mesh
    fw64MaterialCollection material_collection;
};

void fw64_n64_mesh_init(fw64Mesh* mesh);

/** Frees the resources used by this mesh, but does not delete the actual mesh */
void fw64_n64_mesh_uninit(fw64Mesh* mesh, fw64Allocator* allocator);

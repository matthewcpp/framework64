#pragma once

#include "framework64/util/mesh_builder.h"

#include "framework64/n64/material_bundle.h"

#include <nusys.h>

typedef struct {
    Vtx* vertices;
    size_t vertex_count;
    Gfx* display_list;
    size_t display_list_count;
} fw64N64PrimitiveInfo;

struct fw64MeshBuilder {
    fw64N64PrimitiveInfo* primitive_infos;
    size_t primitive_info_count;
    fw64MaterialBundle* material_bundle;

    size_t active_primitive_index;
    fw64Allocator* allocator;
    Box bounding;
    size_t next_image_index;
};
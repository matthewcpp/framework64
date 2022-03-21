#pragma once

/** \file mesh_data_itr.h */

#include "framework64/mesh.h"

#ifdef PLATFORM_N64
    #include <nusys.h>
#endif

typedef struct {
    fw64Mesh* mesh;
    int triangle_index;
#ifdef PLATFORM_N64 
    
    Gfx* display_list;
    Vtx* vertex_cache;
#else
    fw64Primitive* primitive;
#endif
} fw64MeshDataItr;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_mesh_data_itr_init(fw64MeshDataItr* itr, fw64Mesh* mesh, uint32_t primitive_index);
void fw64_mesh_data_itr_uninit(fw64MeshDataItr* itr);

int fw64_mesh_data_itr_has_next(fw64MeshDataItr* itr);
int fw64_mesh_data_itr_next(fw64MeshDataItr* itr);
void fw64_mesh_data_itr_get_triangle_points(fw64MeshDataItr* itr, Vec3* a, Vec3* b, Vec3* c);

#ifdef __cplusplus
}
#endif

#include "framework64/util/mesh_data_itr.h"
#include "framework64/n64/mesh.h"

#include <limits.h>

#define CURRENT_DISPLAY_LIST_OPCODE(itr) (((itr)->display_list->words.w0 & 0xFF000000) >> 24)
#define CACHE_DATA_TRIANGLE_COUNT 1
#define TRIANGLE_INDEX_NONE -1

void fw64_mesh_data_itr_init(fw64MeshDataItr* itr, fw64Mesh* mesh, uint32_t primitive_index) {
    itr->mesh = mesh;

    fw64Primitive* primitive = mesh->primitives + primitive_index;
    itr->display_list = primitive->display_list;
    itr->vertex_cache = NULL;
    itr->triangle_index = 0;
}

void fw64_mesh_data_itr_uninit(fw64MeshDataItr* itr) {}
    


int fw64_mesh_data_itr_has_next(fw64MeshDataItr* itr) {
    return CURRENT_DISPLAY_LIST_OPCODE(itr) != G_ENDDL;
}

static int fw64_n64_mesh_data_itr_next_opcode(fw64MeshDataItr* itr) {
        itr->display_list++;
        itr->triangle_index = TRIANGLE_INDEX_NONE;
        return fw64_mesh_data_itr_next(itr);
}

int fw64_mesh_data_itr_next(fw64MeshDataItr* itr) {
    uint32_t opcode = CURRENT_DISPLAY_LIST_OPCODE(itr);

    switch(opcode) {
        case G_VTX: { // set the current vertex cache and parse the next opcode
            itr->vertex_cache = (Vtx*)itr->display_list->words.w1;
            return fw64_n64_mesh_data_itr_next_opcode(itr);
        }

        case G_TRI1: {
            itr->triangle_index += 1;

            if (itr->triangle_index > 0){
                return fw64_n64_mesh_data_itr_next_opcode(itr);
            }

            return 1;
        }

        case G_TRI2: {
            itr->triangle_index += 1;

            if (itr->triangle_index > 1){
                return fw64_n64_mesh_data_itr_next_opcode(itr);
            }
            
            return 1;
        }
        
        case G_ENDDL:
        default:
            return 0;
    }
}

static void fw64_mesh_data_itr_vtx_to_vec3(Vtx* vtx, Vec3* vec) {
    vec->x = (float)vtx->v.ob[0];
    vec->y = (float)vtx->v.ob[1];
    vec->z = (float)vtx->v.ob[2];
}

void fw64_mesh_data_itr_get_triangle_points(fw64MeshDataItr* itr, Vec3* a, Vec3* b, Vec3* c) {
    uint32_t tri = itr->triangle_index == 0 ? itr->display_list->words.w0 : itr->display_list->words.w1;

    uint32_t idx_a = ((tri & 0x00FF0000) >> 16) / 2;
    uint32_t idx_b = ((tri & 0x0000FF00) >> 8) / 2;
    uint32_t idx_c = (tri & 0x000000FF) / 2;

    fw64_mesh_data_itr_vtx_to_vec3(itr->vertex_cache + idx_a, a);
    fw64_mesh_data_itr_vtx_to_vec3(itr->vertex_cache + idx_b, b);
    fw64_mesh_data_itr_vtx_to_vec3(itr->vertex_cache + idx_c, c);
}


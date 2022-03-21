#include "framework64/util/mesh_data_itr.h"
#include "framework64/desktop/mesh.h"

#include <cstring>

#define TRIANGLE_INDEX_NONE (-1)


void fw64_mesh_data_itr_init(fw64MeshDataItr* itr, fw64Mesh* mesh, uint32_t primitive_index) {
    itr->primitive = &mesh->primitives[primitive_index];
    itr->triangle_index = TRIANGLE_INDEX_NONE;
}

void fw64_mesh_data_itr_uninit(fw64MeshDataItr* itr) {}

int fw64_mesh_data_itr_has_next(fw64MeshDataItr* itr) {
    return (itr->primitive->indices.size() > 0) && (itr->triangle_index < static_cast<int>(itr->primitive->indices.size() / 3));
}

int fw64_mesh_data_itr_next(fw64MeshDataItr* itr) {
    itr->triangle_index += 1;

    return itr->triangle_index < static_cast<int>(itr->primitive->indices.size() / 3);
}

void fw64_mesh_data_itr_get_triangle_points(fw64MeshDataItr* itr, Vec3* a, Vec3* b, Vec3* c) {
    auto const & elements = itr->primitive->indices;

    int triangle_element_index = itr->triangle_index * 3;

    std::memcpy(a, itr->primitive->positions.data() + (elements[triangle_element_index] * 3), sizeof(Vec3));
    std::memcpy(b, itr->primitive->positions.data() + (elements[triangle_element_index + 1] * 3), sizeof(Vec3));
    std::memcpy(c, itr->primitive->positions.data() + (elements[triangle_element_index + 2] * 3), sizeof(Vec3));
}
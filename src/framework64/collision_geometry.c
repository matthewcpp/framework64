#include "framework64/collision_geometry.h"
#include "framework64/types.h"

#include <string.h>

fw64CollisionGeometry* fw64_collision_geometry_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator){
    fw64CollisionGeometry* geometry = (fw64CollisionGeometry*)fw64_allocator_malloc(allocator, sizeof(fw64CollisionGeometry));
    fw64_collision_geometry_init_from_datasource(geometry, data_source, allocator);

    return geometry;
}

void fw64_collision_geometry_init_from_datasource(fw64CollisionGeometry* geometry, fw64DataSource* data_source, fw64Allocator* allocator) {
    fw64_data_source_read(data_source, &geometry->info, sizeof(fw64CollisionGeometryInfo), 1);

    geometry->triangles = fw64_allocator_malloc(allocator, sizeof(fw64CollisionTriangle) * geometry->info.triangle_count);
    fw64_data_source_read(data_source, geometry->triangles, sizeof(fw64CollisionTriangle), geometry->info.triangle_count);

    const size_t cell_count = geometry->info.cell_count_x * geometry->info.cell_count_z;
    geometry->cells = fw64_allocator_malloc(allocator, sizeof(fw64CollisionGeometryCell) * cell_count);
    fw64_data_source_read(data_source, geometry->cells, sizeof(fw64CollisionGeometryCell), cell_count);
}

void fw64_collision_geometry_uninit(fw64CollisionGeometry* geometry, fw64Allocator* allocator) {
    if (geometry->triangles) {
        fw64_allocator_free(allocator, geometry->triangles);
    }
}

int fw64_collision_geometry_query_vec3(fw64CollisionGeometry* geometry, const Vec3* vec, fw64CollisionGeometryQuery* query) {
    query->cell_count = 0;

    // ensure the query point is in the grid.
    if (vec->x < geometry->info.bounding_box.min.x || vec->x > geometry->info.bounding_box.max.x ||
        vec->z < geometry->info.bounding_box.min.z || vec->z > geometry->info.bounding_box.max.z) {
        return query->cell_count;
    }

    uint32_t cell_x = 0;//fw64_floorf((vec->x - geometry->info.bounding_box.min.x) / (float)geometry->info.cell_count_x);
    uint32_t cell_z = 0;//fw64_floorf((vec->z - geometry->info.bounding_box.min.z) / (float)geometry->info.cell_count_z);

    uint32_t cell_index = cell_z * geometry->info.cell_count_z + cell_x;
    query->cells[query->cell_count++] = geometry->cells + cell_index;

    return query->cell_count;
}

void fw64_collision_geometry_delete(fw64CollisionGeometry* geometry, fw64Allocator* allocator) {
    fw64_collision_geometry_uninit(geometry, allocator);
    fw64_allocator_free(allocator, geometry);
}

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

    if (geometry->cells) {
        fw64_allocator_free(allocator, geometry->cells);
    }
}

int fw64_collision_geometry_get_cell_coordinates_vec3(const fw64CollisionGeometry* geometry, const Vec3* vec, IVec2* out) {
    // ensure the query point is in the grid.
    if (vec->x < geometry->info.bounds_min.x || vec->x > geometry->info.bounds_max.x ||
        vec->z < geometry->info.bounds_min.y || vec->z > geometry->info.bounds_max.y) {
        return 0;
    }

    const float cell_size_x = (geometry->info.bounds_max.x - geometry->info.bounds_min.x) / geometry->info.cell_count_x;
    const float cell_size_z = (geometry->info.bounds_max.y - geometry->info.bounds_min.y) / geometry->info.cell_count_z;

    out->x = (int)fw64_floorf((vec->x - geometry->info.bounds_min.x) / cell_size_x);
    out->y = (int)fw64_floorf((vec->z - geometry->info.bounds_min.y) / cell_size_z);

    return 1;
}

int fw64_collision_geometry_query_vec3(fw64CollisionGeometry* geometry, const Vec3* vec, fw64CollisionGeometryQuery* query) {
    query->cell_count = 0;
    IVec2 coords;
    if (fw64_collision_geometry_get_cell_coordinates_vec3(geometry, vec, &coords)) {
        query->cells[0] = geometry->cells + coords.y * geometry->info.cell_count_x + coords.x;
        query->cell_count = 1;
    }

    return query->cell_count;
}

void fw64_collision_geometry_delete(fw64CollisionGeometry* geometry, fw64Allocator* allocator) {
    fw64_collision_geometry_uninit(geometry, allocator);
    fw64_allocator_free(allocator, geometry);
}

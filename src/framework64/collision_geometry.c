#include "framework64/collision_geometry.h"

#include "framework64/collision.h"
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

    geometry->ladders = fw64_allocator_malloc(allocator, sizeof(fw64CollisionLadder) * geometry->info.ladder_count);
    fw64_data_source_read(data_source, geometry->ladders, sizeof(fw64CollisionLadder), geometry->info.ladder_count);

    const size_t cell_count = geometry->info.cell_count_x * geometry->info.cell_count_z;
    geometry->cells = fw64_allocator_malloc(allocator, sizeof(fw64CollisionGeometryCell) * cell_count);
    fw64_data_source_read(data_source, geometry->cells, sizeof(fw64CollisionGeometryCell), cell_count);

    geometry->cell_size.x = (geometry->info.bounding.max.x - geometry->info.bounding.min.x) / geometry->info.cell_count_x;
    geometry->cell_size.y = (geometry->info.bounding.max.y - geometry->info.bounding.min.y) / geometry->info.cell_count_y;
    geometry->cell_size.z = (geometry->info.bounding.max.z - geometry->info.bounding.min.z) / geometry->info.cell_count_z;
}

void fw64_collision_geometry_uninit(fw64CollisionGeometry* geometry, fw64Allocator* allocator) {
    if (geometry->triangles) {
        fw64_allocator_free(allocator, geometry->triangles);
    }

    if (geometry->cells) {
        fw64_allocator_free(allocator, geometry->cells);
    }
}

int fw64_collision_geometry_get_cell_coordinates_vec3(const fw64CollisionGeometry* geometry, const Vec3* vec, IVec3* out) {
    // ensure the query point is in the grid.
    if (vec->x < geometry->info.bounding.min.x || vec->x > geometry->info.bounding.max.x ||
        vec->z < geometry->info.bounding.min.z || vec->z > geometry->info.bounding.max.z) {
        return 0;
    }

    const float cell_size_x = (geometry->info.bounding.max.x - geometry->info.bounding.min.x) / geometry->info.cell_count_x;
    const float cell_size_z = (geometry->info.bounding.max.z - geometry->info.bounding.min.z) / geometry->info.cell_count_z;

    out->x = (int)fw64_floorf((vec->x - geometry->info.bounding.min.x) / cell_size_x);
    out->y = 0;
    out->z = (int)fw64_floorf((vec->z - geometry->info.bounding.min.z) / cell_size_z);

    return 1;
}

fw64CollisionGeometryCell* fw64_collision_geometry_get_cell(const fw64CollisionGeometry* geometry, const IVec3* coords) {
    if (    coords->x < 0 || coords->x > (int)geometry->info.cell_count_x || 
            coords->y < 0 || coords->y > (int)geometry->info.cell_count_y ||
            coords->z < 0 || coords->z > (int)geometry->info.cell_count_z) 
    {
        return NULL;
    }

    return geometry->cells + coords->z * geometry->info.cell_count_x + coords->x;
}

int fw64_collision_geometry_query_vec3(const fw64CollisionGeometry* geometry, const Vec3* vec, fw64CollisionGeometryQuery* query) {
    query->cell_count = 0;
    IVec3 coords;
    if (fw64_collision_geometry_get_cell_coordinates_vec3(geometry, vec, &coords)) {
        query->cells[0] = fw64_collision_geometry_get_cell(geometry, &coords);
        query->cell_count = 1;
    }

    return query->cell_count;
}

int fw64_collision_geometry_query_ray(const fw64CollisionGeometry* geometry, const Vec3* origin, const Vec3* direction, float distance, fw64CollisionGeometryQuery* query) {
    query->cell_count = 0;

    // Convert origin to grid space
    IVec3 cell_coords;
    fw64_collision_geometry_get_cell_coordinates_vec3(geometry, origin, &cell_coords);

    // Precompute step direction (which way we move in grid cells)
    IVec3 step = ivec3_zero();
    step.x = (direction->x > 0) ? 1 : -1;
    step.y = (direction->y > 0) ? 1 : -1;
    step.z = (direction->z > 0) ? 1 : -1;

    // 3. Compute initial tMax (distance to next boundary) and tDelta (distance between boundaries)
    Vec3 t_max;
    Vec3 t_delta;

    const int *cell_coords_ptr = (int*)&cell_coords, *step_ptr = (int*)&step;
    const float *origin_ptr = (float*)origin, *dir_ptr = (float*)direction;
    float *t_max_ptr = (float*)&t_max, *t_delta_ptr = (float*)&t_delta, *cell_size_ptr = (float*)&geometry->cell_size, *bounding_min_ptr = (float*)&geometry->info.bounding.min;

    for (int axis = 0; axis < 3; axis++) {
        if (dir_ptr[axis] != 0.0f) {
            float cellBoundary = bounding_min_ptr[axis] + ((cell_coords_ptr[axis] + (step_ptr[axis] > 0 ? 1 : 0)) * cell_size_ptr[axis]);
            t_max_ptr[axis] = (cellBoundary - origin_ptr[axis]) / dir_ptr[axis];
            t_delta_ptr[axis] = cell_size_ptr[axis] / fw64_fabsf(dir_ptr[axis]);
        } else {
            t_max_ptr[axis] = FLT_MAX;
            t_delta_ptr[axis] = FLT_MAX;
        }
    }

    float traveled = 0.0;

    // 4. Walk cells until ray length exceeded
    while (traveled <= distance) {
        fw64CollisionGeometryCell* cell = fw64_collision_geometry_get_cell(geometry, &cell_coords);
        if (!cell) {
            break;
        }

        query->cells[query->cell_count++] = cell;

        if (query->cell_count == FW64_COLLISION_GEOMETRY_QUERY_MAX_CELL_COUNT) {
            break;
        }

        // Pick smallest tMax to know which axis boundary we cross next
        if (t_max.x < t_max.y && t_max.x < t_max.z) {
            cell_coords.x += step.x;
            traveled = t_max.x;
            t_max.x += t_delta.x;
        }
        else if (t_max.y < t_max.z) {
            cell_coords.y += step.y;
            traveled = t_max.y;
            t_max.y += t_delta.y;
        }
        else {
            cell_coords.z += step.z;
            traveled = t_max.z;
            t_max.z += t_delta.z;
        }
    }

    return query->cell_count;
}

typedef struct {
    float closest_t;
    Vec3 closest_pt;
    const fw64CollisionTriangle* closest_triangle;
} fw64CollisionGeometryTraingleRaycast;

static void fw64_collision_geometry_test_triangles(fw64CollisionGeometryTraingleRaycast* raycast, const fw64CollisionTriangle* triangles, uint32_t triangle_count, const Vec3* origin, const Vec3* direction, float min_t) {
    float current_t;
    Vec3 current_pt;

    if (triangle_count == 0) {
        return;
    }

    for (uint32_t t = 0; t < triangle_count; t++) {
        const fw64CollisionTriangle* triangle = triangles + t;

        if (fw64_collision_test_ray_triangle(origin, direction, &triangle->A, &triangle->B, &triangle->C, &current_pt, &current_t)) {
            if (current_t > min_t && current_t < raycast->closest_t) {
                raycast->closest_t = current_t;
                raycast->closest_pt = current_pt;
                raycast->closest_triangle = triangle;
            }
        }
    }
}
#ifdef FW64_COLLISION_GEOMETRY_DEBUG_INFO
#define fw64_collision_geometry_reset_last_raycast_test_count(geometry) ((fw64CollisionGeometry*)geometry)->last_raycast_test_count = 0
#define fw64_collision_geometry_increment_last_raycast_test_count(geometry, count) ((fw64CollisionGeometry*)geometry)->last_raycast_test_count += (count)
#else
#define fw64_collision_geometry_reset_last_raycast_test_count(geometry)
#define fw64_collision_geometry_increment_last_raycast_test_count(geometry, count)
#endif
const fw64CollisionTriangle* fw64_collision_geometry_raycast_triangle(const fw64CollisionGeometry* geometry, const Vec3* origin, const Vec3* direction, float min_distance, float max_distance, fw64CollisionGeometryType type_mask, Vec3* out_pt) {
    fw64_collision_geometry_reset_last_raycast_test_count(geometry);

    fw64CollisionGeometryQuery query;
    if (!fw64_collision_geometry_query_ray(geometry, origin, direction, max_distance, &query)) {
        return NULL;
    }

    fw64CollisionGeometryTraingleRaycast raycast;
    raycast.closest_t = max_distance;
    raycast.closest_triangle = NULL;

    for (uint32_t c = 0; c < query.cell_count; c++) {
        fw64CollisionGeometryCell* cell = query.cells[c];

        if (type_mask & FW64_COLLISION_GEOMETRY_TYPE_FLOOR) {
            fw64_collision_geometry_test_triangles(&raycast, geometry->triangles + cell->floor_index, cell->floor_count, origin, direction, min_distance);
            fw64_collision_geometry_increment_last_raycast_test_count(geometry, cell->floor_count);
        }

        if (type_mask & FW64_COLLISION_GEOMETRY_TYPE_WALL) {
             fw64_collision_geometry_test_triangles(&raycast, geometry->triangles + cell->wall_index, cell->wall_count, origin, direction, min_distance);
            fw64_collision_geometry_increment_last_raycast_test_count(geometry, cell->wall_count);
            }

        if (type_mask & FW64_COLLISION_GEOMETRY_TYPE_CEILING) {
            fw64_collision_geometry_test_triangles(&raycast, geometry->triangles + cell->ceiling_index, cell->ceiling_count, origin, direction, min_distance);
            fw64_collision_geometry_increment_last_raycast_test_count(geometry, cell->ceiling_count);
        }
    }

    *out_pt = raycast.closest_pt;
    return raycast.closest_triangle;
}

void fw64_collision_geometry_delete(fw64CollisionGeometry* geometry, fw64Allocator* allocator) {
    fw64_collision_geometry_uninit(geometry, allocator);
    fw64_allocator_free(allocator, geometry);
}

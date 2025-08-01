#pragma once

#include "framework64/allocator.h"
#include "framework64/box.h"
#include "framework64/data_io.h"
#include "framework64/vec2.h"
#include "framework64/vec3.h"

#include <stdint.h>

typedef struct {
    Vec3 A, B, C, N;
} fw64CollisionTriangle;

typedef struct {
    uint32_t wall_index;
    uint32_t wall_count;
    uint32_t floor_index;
    uint32_t floor_count;
    uint32_t ceiling_index;
    uint32_t ceiling_count;
} fw64CollisionGeometryCell;

typedef struct {
    uint32_t triangle_count;
    uint32_t cell_count_x;
    uint32_t cell_count_z;
    Vec2 bounds_min;
    Vec2 bounds_max;
} fw64CollisionGeometryInfo;

typedef struct {
    fw64CollisionGeometryInfo info;

    fw64CollisionTriangle* triangles;
    fw64CollisionGeometryCell* cells;
} fw64CollisionGeometry;

typedef struct {
    fw64CollisionGeometryCell* cells[4];
    uint32_t cell_count;
} fw64CollisionGeometryQuery;

typedef enum {
    FW64_COLLISION_GEOMETRY_TYPE_FLOOR,
    FW64_COLLISION_GEOMETRY_TYPE_WALL,
    FW64_COLLISION_GEOMETRY_TYPE_CEILING
} fw64CollisionGeometryType;

#ifdef __cplusplus
extern "C" {
#endif

fw64CollisionGeometry* fw64_collision_geometry_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator);
void fw64_collision_geometry_init_from_datasource(fw64CollisionGeometry* geometry, fw64DataSource* data_source, fw64Allocator* allocator);

void fw64_collision_geometry_uninit(fw64CollisionGeometry* geometry, fw64Allocator* allocator);
void fw64_collision_geometry_delete(fw64CollisionGeometry* geometry, fw64Allocator* allocator);

int fw64_collision_geometry_get_cell_coordinates_vec3(const fw64CollisionGeometry* geometry, const Vec3* vec, IVec2* out);
int fw64_collision_geometry_query_vec3(fw64CollisionGeometry* geometry, const Vec3* vec, fw64CollisionGeometryQuery* query);

#define fw64_collision_geometry_get_cell_count(geometry) ((geometry)->info.cell_count_x * (geometry)->info.cell_count_z)
#define fw64_collision_geometry_get_cell_index(geometry, x, z) ((z) * (geometry)->info.cell_count_x + (x))

#ifdef __cplusplus
}
#endif

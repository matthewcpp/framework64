#pragma once

#include "framework64/allocator.h"
#include "framework64/box.h"
#include "framework64/data_io.h"
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
    uint32_t cell_count;
    Box bounding_box;
} fw64CollisionGeometryInfo;

typedef struct {
    fw64CollisionGeometryInfo info;

    fw64CollisionTriangle* triangles;
    fw64CollisionGeometryCell* cells;
} fw64CollisionGeometry;

#ifdef __cplusplus
extern "C" {
#endif

fw64CollisionGeometry* fw64_collision_geometry_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator);
void fw64_collision_geometry_init_from_datasource(fw64CollisionGeometry* geometry, fw64DataSource* data_source, fw64Allocator* allocator);

void fw64_collision_geometry_uninit(fw64CollisionGeometry* geometry, fw64Allocator* allocator);
void fw64_collision_geometry_delete(fw64CollisionGeometry* geometry, fw64Allocator* allocator);

#ifdef __cplusplus
}
#endif

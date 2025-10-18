#pragma once

#include "framework64/allocator.h"
#include "framework64/box.h"
#include "framework64/data_io.h"
#include "framework64/vec2.h"
#include "framework64/vec3.h"

#include <stdint.h>

typedef struct {
    Vec3 A, B, C, N;
    float minY, maxY;
} fw64CollisionTriangle;

typedef struct {
    Vec3 entrance, exit;
    Vec3 normal;
    float radius;
} fw64CollisionLadder;

typedef struct {
    uint16_t type;
    uint16_t node_index;
    uint16_t floor_index;
    uint16_t floor_count;
    uint16_t wall_index;
    uint16_t wall_count;
    uint16_t ceiling_index;
    uint16_t ceiling_count;
    Box primitive; // TODO: make this a union once additional primitives are supported
} fw64CollisionGeometryBoundingVolume;

#define fw64_collision_geometry_bounding_volume_triangle_count(volume) ((volume)->floor_count + (volume)->wall_count + (volume)->ceiling_count)

typedef struct {
    uint16_t ladder_index;
    uint16_t ladder_count;
    uint16_t bounding_volume_index;
    uint16_t bounding_volume_count;
} fw64CollisionGeometryCell;

typedef struct {
    uint32_t triangle_count;
    uint32_t ladder_count;
    uint32_t bounding_volume_count;
    uint32_t cell_count_x;
    uint32_t cell_count_y;
    uint32_t cell_count_z;
    Box bounding;
} fw64CollisionGeometryInfo;

#ifdef FW64_COLLISION_GEOMETRY_DEBUG_INFO
typedef struct {
    uint32_t triangles_considered;
    uint32_t triangles_skipped;
    uint32_t triangles_checked;
} fw64GeometryDataDebugInfo;

#define fw64_geometry_debug_info_increment_triangles_considered(info, count) (info)->triangles_considered += (count)
#define fw64_geometry_debug_info_increment_triangles_skipped(info, count) (info)->triangles_skipped += (count)
#define fw64_geometry_debug_info_increment_triangles_checked(info, count) (info)->triangles_checked += (count)

#else

#define fw64_geometry_debug_info_increment_triangles_considered(info, count) 
#define fw64_geometry_debug_info_increment_triangles_skipped(info, count) 
#define fw64_geometry_debug_info_increment_triangles_checked(info, count) 

#endif

typedef struct {
    fw64CollisionGeometryInfo info;
    fw64CollisionTriangle* triangles;
    fw64CollisionLadder* ladders;
    fw64CollisionGeometryBoundingVolume* bounding_volumes;
    fw64CollisionGeometryCell* cells;
    Vec3 cell_size;

#ifdef FW64_COLLISION_GEOMETRY_DEBUG_INFO
    fw64GeometryDataDebugInfo last_raycast_debug_info;
#endif
} fw64CollisionGeometry;

#define FW64_COLLISION_GEOMETRY_QUERY_MAX_CELL_COUNT 8

typedef struct {
    fw64CollisionGeometryCell* cells[FW64_COLLISION_GEOMETRY_QUERY_MAX_CELL_COUNT];
    uint32_t cell_count;
} fw64CollisionGeometryQuery;

typedef enum {
    FW64_COLLISION_GEOMETRY_TYPE_FLOOR      = 1 << 0,
    FW64_COLLISION_GEOMETRY_TYPE_WALL       = 1 << 1,
    FW64_COLLISION_GEOMETRY_TYPE_CEILING    = 1 << 2
} fw64CollisionGeometryType;

#ifdef __cplusplus
extern "C" {
#endif

fw64CollisionGeometry* fw64_collision_geometry_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator);
void fw64_collision_geometry_init_from_datasource(fw64CollisionGeometry* geometry, fw64DataSource* data_source, fw64Allocator* allocator);

void fw64_collision_geometry_uninit(fw64CollisionGeometry* geometry, fw64Allocator* allocator);
void fw64_collision_geometry_delete(fw64CollisionGeometry* geometry, fw64Allocator* allocator);

int fw64_collision_geometry_get_cell_coordinates_vec3(const fw64CollisionGeometry* geometry, const Vec3* vec, IVec3* out);
int fw64_collision_geometry_query_vec3(const fw64CollisionGeometry* geometry, const Vec3* vec, fw64CollisionGeometryQuery* query);
int fw64_collision_geometry_query_ray(const fw64CollisionGeometry* geometry, const Vec3* origin, const Vec3* direction, float distance, fw64CollisionGeometryQuery* query);

const fw64CollisionTriangle* fw64_collision_geometry_raycast_triangle(const fw64CollisionGeometry* geometry, const Vec3* origin, const Vec3* direction, float min_distance, float max_distance, fw64CollisionGeometryType type_mask, Vec3* out_pt);
/** 
 * Returns the cell from the grid with the supplied indicies. 
 * Will return NULL if the indices are invalid
*/
fw64CollisionGeometryCell* fw64_collision_geometry_get_cell(const fw64CollisionGeometry* geometry, const IVec3* coords);

#define fw64_collision_geometry_get_cell_count(geometry) ((geometry)->info.cell_count_x * (geometry)->info.cell_count_z)
#define fw64_collision_geometry_get_cell_index(geometry, x, y, z) ((z) * (geometry)->info.cell_count_x + (x))

#ifdef __cplusplus
}
#endif

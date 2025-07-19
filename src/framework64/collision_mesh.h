#pragma once

/** \file collision_mesh.h */

#include "framework64/allocator.h"
#include "framework64/box.h"
#include "framework64/data_io.h"
#include "framework64/transform.h"
#include "framework64/vec3.h"

#include <stdint.h>

typedef struct {
    uint32_t point_count;

    /** This is the total number of items in elements The total number of triangles is element_count / 3 */
    uint32_t element_count;
    Box box;
    Vec3* points;
    uint16_t* elements;
} fw64CollisionMesh;

#ifdef __cplusplus
extern "C" {
#endif


int fw64_collision_mesh_load_from_datasource(fw64CollisionMesh* collision_mesh, fw64DataSource* data_source, fw64Allocator* allocator);
void fw64_collision_mesh_uninit(fw64CollisionMesh* collision_mesh, fw64Allocator* allocator);

int fw64_collision_mesh_test_sphere(const fw64CollisionMesh* collision_mesh, fw64Transform* transform, Vec3* center, float radius, Vec3* out_point);
int fw64_collision_mesh_test_ray(const fw64CollisionMesh* collision_mesh, fw64Transform* transform, Vec3* origin, Vec3* direction, Vec3* out_point, float* out_dist);

#define fw64_collision_mesh_get_triangle_count(collision_mesh) ((collision_mesh)->element_count / 3)

#ifdef __cplusplus
}
#endif

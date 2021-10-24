#pragma once

/** \file collide.h */

#include "framework64/box.h"
#include "framework64/vec3.h"

#ifdef __cplusplus
extern "C" {
#endif


void fw64_closest_point_to_triangle(Vec3* point, Vec3* a, Vec3* b, Vec3* c, Vec3* out);

/**
 * Returns true if sphere s intersects triangle ABC, false otherwise.
 * The point p on abc closest to the sphere center is also returned
 */
int fw64_collision_test_sphere_triangle(Vec3* center, float radius, Vec3* a, Vec3* b, Vec3* c, Vec3* point);


/**
 * Returns nonzero value if the sphere intersects AABB, otherwise 0
 * Additionally computes point on the AABB closest to sphere center
 */
int fw64_collision_test_box_sphere(Box* box, Vec3* center, float radius, Vec3* point);

/**
 * Returns nonzero value if the ray intersects the triangle, otherwise 0
 * Additional computes the intersection point and distance from ray origin of the intersection
 */
int fw64_collision_test_ray_triangle(Vec3* origin, Vec3* direction, Vec3* a, Vec3* b, Vec3* c, Vec3* point, float* t);


int fw64_collision_test_ray_box(Vec3* origin, Vec3* direction, Box* box, Vec3* point, float* t);

#ifdef __cplusplus
}
#endif
#pragma once

/** \file collision.h */

#include "framework64/box.h"
#include "framework64/capsule.h"
#include "framework64/vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

void fw64_closest_point_to_triangle(const Vec3* point, const Vec3* a, const Vec3* b, const Vec3* c, Vec3* out);

void fw64_closest_point_on_line_segment(const Vec3* a, const Vec3* b, const Vec3* point, Vec3* out);

/** Finds the closest points between segment 1 (p1 to q1) and segment 2 (p2 to q2).
 * Stores the closest point on seg 1 in c1, and the closest point on seg 2 in c2.
 * Returns the squared distance between c1 and c2.
 */
float fw64_closest_points_segment_segment(
    const Vec3* p1, const Vec3* q1, 
    const Vec3* p2, const Vec3* q2, 
    Vec3* c1, Vec3* c2);

/**
 * Returns true if sphere s intersects triangle ABC, false otherwise.
 * The point p on abc closest to the sphere center is also returned
 */
int fw64_collision_test_sphere_triangle(const Vec3* center, float radius, const Vec3* a, const Vec3* b, const Vec3* c, Vec3* point);

int fw64_collision_test_capsule_triangle(const fw64Capsule* capsule, const Vec3* tri_a, const Vec3* tri_b, const Vec3* tri_c, const Vec3* n, Vec3* out_tri_point, Vec3* out_capsule_point);

/**
 * Returns nonzero value if the sphere intersects AABB, otherwise 0
 * Additionally computes point on the AABB closest to sphere center
 */
int fw64_collision_test_box_sphere(const Box* box, const Vec3* center, float radius, Vec3* point);

/**
 * Returns nonzero value if the ray intersects the triangle, otherwise 0
 * Additionally computes the intersection point and distance from ray origin of the intersection
 */
int fw64_collision_test_ray_triangle(const Vec3* origin, const Vec3* direction, const Vec3* a, const Vec3* b, const Vec3* c, Vec3* point, float* t);

/**
 * Returns nonzero value if the ray intersects the box, otherwise 0
 * Additionally computes the intersection point and distance from ray origin of the intersection
 */
int fw64_collision_test_ray_box(const Vec3* origin, const Vec3* direction, const Box* box, Vec3* point, float* t);

/**
 * Returns nonzero value if the ray intersects the sphere, otherwise 0
 * Additionally computes the intersection point (point) and distance (t) from ray origin of the intersection
 * If no intersection, point and t are undefined
 */
int fw64_collision_test_ray_sphere(const Vec3* origin, const Vec3* direction, const Vec3* center, float radius, Vec3* point, float* t);

/**
 * Returns nonzero value if the ray intersects the capsule defined by the line between Point A and Point B, and the radius
 * Returns 0 if no intersection found
 * Computes the intersection point and distance (t) from ray origin to intersection.
 * If no intersection, out_point and t are undefined
 */
int fw64_collision_test_ray_capsule(Vec3* origin, Vec3* direction,
                                    Vec3* point_a, Vec3* point_b, float radius,
                                    Vec3* out_point, float* out_t);

/**
 * Returns nonzero value if the moving sphere (given by center, radius, and direction) intersects the AABB (b)
 * Returns 0 if no intersection found
 * Computes the intersection point (out_point) and time (out_t).
 * If no intersection, out_point and out_t are undefined
 */
int fw64_collision_test_moving_sphere_box(Vec3* center, float radius, Vec3* direction,
                                          Box* b,
                                          Vec3* out_point, float* out_t);

/**
 * Intersect AABBs a and b moving with constant velocities va and vb.
 * On intersection, return time of first and last contact in tfirst and tlast
 * Note: The implementation, as taken from Ericson's RT Collision detection will return true if va == vb
 */
int fw64_collision_test_moving_boxes(Box* a, Vec3* va, Box* b, Vec3* vb, float* tfirst, float* tlast);

/**
 * Intersect spheres a (center ca and radius ra) and b (center cb and radius rb) with constant velocities va and vb.
 * Returns non-zero if an intersection exists, or zero if none
 * Time of first intersection (if any) is stored in t. If no intersection, t is undefined
 */
int fw64_collision_test_moving_spheres(Vec3* ca, float ra, Vec3* va, Vec3* cb, float rb, Vec3* vb, float* t);

void fw64_collision_get_normal_box_point(Vec3* point, Box* box, Vec3* out_normal);

#ifdef __cplusplus
}
#endif

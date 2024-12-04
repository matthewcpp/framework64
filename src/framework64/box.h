#pragma once

/** \file box.h */

#include "vec3.h"

typedef struct {
    Vec3 min;
    Vec3 max;
} Box;

#ifdef __cplusplus
extern "C" {
#endif

void box_set_center_extents(Box* box, Vec3* center, Vec3* extents);

void box_center(Box* box, Vec3* out);
void box_size(Box* box, Vec3* out);
void box_extents(Box* box, Vec3* out);
void box_invalidate(Box* box);
void box_encapsulate_point(Box* box, const Vec3* pt);
void box_encapsulate_box(Box* box, const Box* to_encapsulate);
int box_intersection(const Box* a, const Box* b);
int box_contains_point(Box* box, Vec3* pt);
/**
 * Compute Point on or in AABB that is closest to supplied point p
 */
void box_closest_point(const Box* box, const Vec3* point, Vec3* out);

void matrix_transform_box(float* matrix, Box* box, Box* out);

#ifdef __cplusplus
}
#endif

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

void box_center(Box* box, Vec3* out);
void box_size(Box* box, Vec3* out);
void box_extents(Box* box, Vec3* out);
void box_invalidate(Box* box);
void box_encapsulate_point(Box* box, Vec3* pt);
void box_encapsulate_box(Box* box, Box* to_encapsulate);
int box_intersection(Box* a, Box* b);

void matrix_transform_box(float* matrix, Box* box, Box* out);

#ifdef __cplusplus
}
#endif

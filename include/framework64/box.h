#ifndef FW64_BOX_H
#define FW64_BOX_H

#include "vec3.h"

typedef struct {
    Vec3 min;
    Vec3 max;
} Box;

void box_center(Box* box, Vec3* out);
void box_size(Box* box, Vec3* out);
void box_extents(Box* box, Vec3* out);
void box_invalidate(Box* box);
void box_encapsulate_point(Box* box, Vec3* pt);
void box_encapsulate_box(Box* box, Box* to_encapsulate);
int box_intersection(Box* a, Box* b);

void matrix_transform_box(float* matrix, Box* box, Box* out);

#endif
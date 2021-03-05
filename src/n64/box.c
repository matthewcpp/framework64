#include "framework64/box.h"

#include <math.h>
#include <string.h>


void box_center(Box* box, Vec3* out) {
    vec3_add(out, &box->min, &box->max);
    vec3_scale(out, out, 0.5f);
}

void box_size(Box* box, Vec3* out) {
    out->x = fabsf(box->max.x - box->min.x);
    out->y = fabsf(box->max.y - box->min.y);
    out->z = fabsf(box->max.z - box->min.z);
}

void box_extents(Box* box, Vec3* out) {
    box_size(box, out);
    vec3_scale(out, out, 0.5f);
}

void box_invalidate(Box* box) {
    box->min.x = FLT_MAX;
    box->min.y = FLT_MAX;
    box->min.z = FLT_MAX;

    box->max.x = FLT_MIN;
    box->max.y = FLT_MIN;
    box->max.z = FLT_MIN;
}

void box_encapsulate_point(Box* box, Vec3* pt) {
    if (pt->x < box->min.x)
         box->min.x = pt->x;
    if (pt->y < box->min.y)
         box->min.y = pt->y;
    if (pt->z < box->min.z)
         box->min.z = pt->z;

    if (pt->x > box->max.x)
        box->max.x = pt->x;
    if (pt->y > box->max.y)
        box->max.y = pt->y;
    if (pt->z > box->max.z)
        box->max.z = pt->z;
}

void box_encapsulate_box(Box* box, Box* to_encapsulate) {
    box_encapsulate_point(box, &to_encapsulate->min);
    box_encapsulate_point(box, &to_encapsulate->max);
}

// Based on code from: https://github.com/erich666/GraphicsGems/blob/master/gems/TransBox.c
void matrix_transform_box(float* matrix, Box* box, Box* out) {
    float* box_min = &box->min.x;
    float* box_max = &box->max.x;

    float* out_min = &out->min.x;
    float* out_max = &out->max.x;

    /* Take care of translation by beginning at T. */
    memcpy(&out->min, matrix + 12, 3 * sizeof(float));
    memcpy(&out->max, matrix + 12, 3 * sizeof(float));
    
    /* Now find the extreme points by considering the product of the min and max with each component of M.  */
        for(int i = 0; i < 3; i++ ) {
            for (int j = 0; j < 3; j++) {
                float a = (matrix[i * 4 + j] * box_min[j]);
                float b = (matrix[i * 4 + j] * box_max[j]);
                if (a < b) {
                    out_min[i] += a;
                    out_max[i] += b;
                } else {
                    out_min[i] += b;
                    out_max[i] += a;
                }
            }
        }
}

int box_intersection(Box* a, Box* b) {
    if (a->max.x < b->min.x || a->min.x > b->max.x) return 0;
    if (a->max.y < b->min.y || a->min.y > b->max.y) return 0;
    if (a->max.z < b->min.z || a->min.z > b->max.z) return 0;

    return 1;
}

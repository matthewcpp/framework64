#include "framework64/box.h"

#include <float.h>

void box_center(Box* box, Vec3* out) {
    vec3_add(out, &box->min, &box->max);
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
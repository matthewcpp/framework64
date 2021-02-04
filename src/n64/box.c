#include "framework64/box.h"

void box_center(Box* box, Vec3* out) {
    vec3_add(out, &box->min, &box->max);
    vec3_scale(out, out, 0.5f);
}
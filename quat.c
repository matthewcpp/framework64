#include "quat.h"

#include <math.h>

void quat_ident(Quat* q) {
    q->x = 0.0f;
    q->y = 0.0f;
    q->z = 0.0f;
    q->w = 1.0f;
}

void quat_set_axis_angle(Quat* out, float x, float y, float z, float rad) {
    rad = rad * 0.5;
    float s = _nsinf(rad);
    out->x = s * x;
    out->y = s * y;
    out->z = s * z;
    out->w = _ncosf(rad);
}

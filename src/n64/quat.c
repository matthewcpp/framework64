#include "framework64/quat.h"

#include <math.h>

#ifndef M_PI
    #define M_PI FVAL_PI
#endif

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

void quat_transform_vec3(Vec3* out, Quat* q, Vec3* a) {
    float qx = q->x, qy = q->y, qz = q->z, qw = q->w;
    float x = a->x, y =a->y, z = a->z;

    // var qvec = [qx, qy, qz];
    // var uv = vec3.cross([], qvec, a);

    float uvx = qy * z - qz * y,
    uvy = qz * x - qx * z,
    uvz = qx * y - qy * x;

    // var uuv = vec3.cross([], qvec, uv);

    float uuvx = qy * uvz - qz * uvy,
    uuvy = qz * uvx - qx * uvz,
    uuvz = qx * uvy - qy * uvx;

    // vec3.scale(uv, uv, 2 * w);

    float w2 = qw * 2;
    uvx *= w2;
    uvy *= w2;
    uvz *= w2;

    // vec3.scale(uuv, uuv, 2);

    uuvx *= 2;
    uuvy *= 2;
    uuvz *= 2;

    // return vec3.add(out, a, vec3.add(out, uv, uuv));

    out->x = x + uvx + uuvx;
    out->y = y + uvy + uuvy;
    out->z = z + uvz + uuvz;
}

void quat_normalize(Quat* q) {
    float len = q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w;

    if (len > 0) {
        len = 1.0f / _nsqrtf(len);
    }

    q->x *= len;
    q->y *= len;
    q->z *= len;
    q->w *= len;
}

void quat_from_euler(Quat* q, float x, float y, float z) {
    float halfToRad = (0.5f * M_PI) / 180.0f;

    x *= halfToRad;
    y *= halfToRad;
    z *= halfToRad;

    float sx = _nsinf(x);
    float cx = _ncosf(x);
    float sy = _nsinf(y);
    float cy = _ncosf(y);
    float sz = _nsinf(z);
    float cz = _ncosf(z);

    q->x = sx * cy * cz - cx * sy * sz;
    q->y = cx * sy * cz + sx * cy * sz;
    q->z = cx * cy * sz - sx * sy * cz;
    q->w = cx * cy * cz + sx * sy * sz;
}
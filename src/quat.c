#include "framework64/quat.h"

#include "framework64/types.h"

void quat_ident(Quat* q) {
    q->x = 0.0f;
    q->y = 0.0f;
    q->z = 0.0f;
    q->w = 1.0f;
}

void quat_conjugate(Quat* q) {
    q->x = -q->x;
    q->y = -q->y;
    q->z = -q->z;
    // q->w = q->w;
}

void quat_set(Quat* q, float x, float y, float z, float w) {
    q->x = x;
    q->y = y;
    q->z = z;
    q->w = w;
}

void quat_set_axis_angle(Quat* out, float x, float y, float z, float rad) {
    rad = rad * 0.5;
    float s = fw64_sinf(rad);
    out->x = s * x;
    out->y = s * y;
    out->z = s * z;
    out->w = fw64_cosf(rad);
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
        len = 1.0f / fw64_sqrtf(len);
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

    float sx = fw64_sinf(x);
    float cx = fw64_cosf(x);
    float sy = fw64_sinf(y);
    float cy = fw64_cosf(y);
    float sz = fw64_sinf(z);
    float cz = fw64_cosf(z);

    q->x = sx * cy * cz - cx * sy * sz;
    q->y = cx * sy * cz + sx * cy * sz;
    q->z = cx * cy * sz - sx * sy * cz;
    q->w = cx * cy * cz + sx * sy * sz;
}
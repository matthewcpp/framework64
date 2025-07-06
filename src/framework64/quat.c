#include "framework64/quat.h"

#include "framework64/types.h"

#include <math.h>

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
    rad = rad * 0.5f;
    float s = fw64_sinf(rad);
    out->x = s * x;
    out->y = s * y;
    out->z = s * z;
    out->w = fw64_cosf(rad);
}

void quat_transform_vec3(const Quat* q, const Vec3* a, Vec3* out) {
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

#ifdef FW64_PLATFORM_N64_LIBULTRA
#include <stdint.h>

int signbitf_polyfill(float x) {
    union { float f; uint32_t i; } u = { x };
    return (u.i >> 31) != 0;
}

float copysignf_polyfill(float x, float y) {
    return (y < 0.0f || (y == 0.0f && signbitf_polyfill(y))) ? -fabsf(x) : fabsf(x);
}

#define fw64_copysignf copysignf_polyfill
#else
#define fw64_copysignf copysignf
#endif

Vec3 quat_to_euler(const Quat* q) {
    Vec3 euler;

    // Roll (Z-axis rotation)
    float sinr_cosp = 2.0f * (q->w * q->x + q->y * q->z);
    float cosr_cosp = 1.0f - 2.0f * (q->x * q->x + q->y * q->y);
    euler.z = atan2f(sinr_cosp, cosr_cosp);

    // Pitch (X-axis rotation)
    float sinp = 2.0f * (q->w * q->y - q->z * q->x);
    if (fabsf(sinp) >= 1.0f)
        euler.x = fw64_copysignf(M_PI / 2.0f, sinp);
    else
        euler.x = asinf(sinp);

    // Yaw (Y-axis rotation)
    float siny_cosp = 2.0f * (q->w * q->z + q->x * q->y);
    float cosy_cosp = 1.0f - 2.0f * (q->y * q->y + q->z * q->z);
    euler.y = atan2f(siny_cosp, cosy_cosp);

    return euler;
}

void quat_slerp(const Quat* a, const Quat* b_in, float t, Quat* out) {
    float omega, cosom, sinom, scale0, scale1;
    Quat b = *b_in;

    // calc cosine
    cosom = a->x * b.x + a->y * b.y + a->z * b.z + a->w * b.w;

    // adjust signs (if necessary)
    if (cosom < 0.0) {
        cosom = -cosom;
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        b.w = -b.w;
    }

    // calculate coefficients
    if (1.0f - cosom > EPSILON) {
        // standard case (slerp)
        omega = fw64_acosf(cosom);
        sinom = fw64_sinf(omega);
        scale0 = fw64_sinf((1.0f - t) * omega) / sinom;
        scale1 = fw64_sinf(t * omega) / sinom;
    } else {
        // "from" and "to" quaternions are very close
        //  ... so we can do a linear interpolation
        scale0 = 1.0f - t;
        scale1 = t;
    }

    // calculate final values
    out->x = scale0 * a->x + scale1 * b.x;
    out->y = scale0 * a->y + scale1 * b.y;
    out->z = scale0 * a->z + scale1 * b.z;
    out->w = scale0 * a->w + scale1 * b.w;
}

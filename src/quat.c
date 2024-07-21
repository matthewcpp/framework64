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

// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Source_code_2
Vec3 quat_to_euler(Quat* q) {
    Vec3 angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q->w * q->x + q->y * q->z);
    double cosr_cosp = 1 - 2 * (q->x * q->x + q->y * q->y);
    angles.x = atan2f(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = fw64_sqrtf(1 + 2 * (q->w * q->y - q->x * q->z));
    double cosp = fw64_sqrtf(1 - 2 * (q->w * q->y - q->x * q->z));
    angles.y = 2 * atan2f(sinp, cosp) - M_PI / 2;

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q->w * q->z + q->x * q->y);
    double cosy_cosp = 1 - 2 * (q->y * q->y + q->z * q->z);
    angles.z = atan2f(siny_cosp, cosy_cosp);

    return angles;
}

void quat_slerp(Quat* out, Quat* a, Quat* b, float t) {
    float omega, cosom, sinom, scale0, scale1;

    // calc cosine
    cosom = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;

    // adjust signs (if necessary)
    if (cosom < 0.0) {
        cosom = -cosom;
        b->x = -b->x;
        b->y = -b->y;
        b->z = -b->z;
        b->w = -b->w;
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
    out->x = scale0 * a->x + scale1 * b->x;
    out->y = scale0 * a->y + scale1 * b->y;
    out->z = scale0 * a->z + scale1 * b->z;
    out->w = scale0 * a->w + scale1 * b->w;
}

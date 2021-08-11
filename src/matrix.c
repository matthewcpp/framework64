#include "framework64/matrix.h"

#include "framework64/types.h"

void matrix_set_identity(float* matrix) {
    matrix[0] = 1.0f;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;
    matrix[4] = 0.0f;
    matrix[5] = 1.0f;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;
    matrix[8] = 0.0f;
    matrix[9] = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;
    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}

void matrix_from_trs(float* out, Vec3* v, Quat* q, Vec3* s) {
    float x2 = q->x + q->x;
    float y2 = q->y + q->y;
    float z2 = q->z + q->z;
    float xx = q->x * x2;
    float xy = q->x * y2;
    float xz = q->x * z2;
    float yy = q->y * y2;
    float yz = q->y * z2;
    float zz = q->z * z2;
    float wx = q->w * x2;
    float wy = q->w * y2;
    float wz = q->w * z2;

    out[0] = (1 - (yy + zz)) * s->x;
    out[1] = (xy + wz) * s->x;
    out[2] = (xz - wy) * s->x;
    out[3] = 0;
    out[4] = (xy - wz) * s->y;
    out[5] = (1 - (xx + zz)) * s->y;
    out[6] = (yz + wx) * s->y;
    out[7] = 0;
    out[8] = (xz + wy) * s->z;
    out[9] = (yz - wx) * s->z;
    out[10] = (1 - (xx + yy)) * s->z;
    out[11] = 0;
    out[12] = v->x;
    out[13] = v->y;
    out[14] = v->z;
    out[15] = 1;
}

void matrix_target_to(float* out, Vec3* eye, Vec3* target, Vec3* up) {
    float eyex = eye->x,
    eyey = eye->y,
    eyez = eye->z,

    upx = up->x,
    upy = up->y,
    upz = up->z;

    float z0 = eyex - target->x,
    z1 = eyey - target->y,
    z2 = eyez - target->z;

    float len = z0 * z0 + z1 * z1 + z2 * z2;

    if (len > 0.0f) {
        len = 1.0f / fw64_sqrtf(len);
        z0 *= len;
        z1 *= len;
        z2 *= len;
    }

    float x0 = upy * z2 - upz * z1,
    x1 = upz * z0 - upx * z2,
    x2 = upx * z1 - upy * z0;

    len = x0 * x0 + x1 * x1 + x2 * x2;

    if (len > 0.0f) {
        len = 1.0f / fw64_sqrtf(len);
        x0 *= len;
        x1 *= len;
        x2 *= len;
    }

    out[0] = x0;
    out[1] = x1;
    out[2] = x2;
    out[3] = 0.0f;
    out[4] = z1 * x2 - z2 * x1;
    out[5] = z2 * x0 - z0 * x2;
    out[6] = z0 * x1 - z1 * x0;
    out[7] = 0.0f;
    out[8] = z0;
    out[9] = z1;
    out[10] = z2;
    out[11] = 0.0f;
    out[12] = eyex;
    out[13] = eyey;
    out[14] = eyez;
    out[15] = 1.0f;
}

void matrix_multiply(float* out, float* a, float* b) {
    out[0] = b[0] * a[0] + b[1] * a[4] + b[2] * a[8] + b[3] * a[12];
    out[1] = b[0] * a[1] + b[1] * a[5] + b[2] * a[9] + b[3] * a[13];
    out[2] = b[0] * a[2] + b[1] * a[6] + b[2] * a[10] + b[3] * a[14];
    out[3] = b[0] * a[3] + b[1] * a[7] + b[2] * a[11] + b[3] * a[15];
    out[4] = b[4] * a[0] + b[5] * a[4] + b[6] * a[8] + b[7] * a[12];
    out[5] = b[4] * a[1] + b[5] * a[5] + b[6] * a[9] + b[7] * a[13];
    out[6] = b[4] * a[2] + b[5] * a[6] + b[6] * a[10] + b[7] * a[14];
    out[7] = b[4] * a[3] + b[5] * a[7] + b[6] * a[11] + b[7] * a[15];
    out[8] = b[8] * a[0] + b[9] * a[4] + b[10] * a[8] + b[11] * a[12];
    out[9] = b[8] * a[1] + b[9] * a[5] + b[10] * a[9] + b[11] * a[13];
    out[10] = b[8] * a[2] + b[9] * a[6] + b[10] * a[10] + b[11] * a[14];
    out[11] = b[8] * a[3] + b[9] * a[7] + b[10] * a[11] + b[11] * a[15];
    out[12] = b[12] * a[0] + b[13] * a[4] + b[14] * a[8] + b[15] * a[12];
    out[13] = b[12] * a[1] + b[13] * a[5] + b[14] * a[9] + b[15] * a[13];
    out[14] = b[12] * a[2] + b[13] * a[6] + b[14] * a[10] + b[15] * a[14];
    out[15] = b[12] * a[3] + b[13] * a[7] + b[14] * a[11] + b[15] * a[15];
}

void matrix_perspective(float* out, float fov_degrees, float aspect, float near, float far) {
    float fov_radians = fov_degrees * (M_PI / 180.0f);
    float f = 1.0f / fw64_tanf(fov_radians / 2.0f);
    float nf = 1.0f / (near - far);

    out[0] = f / aspect;
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = f;
    out[6] = 0;
    out[7] = 0;
    out[8] = 0;
    out[9] = 0;
    out[10] = (far + near) * nf;
    out[11] = -1;
    out[12] = 0;
    out[13] = 0;
    out[14] = 2 * far * near * nf;
    out[15] = 0;
}

static float hypot3(float a, float b, float c) {
    return fw64_sqrtf(a * a + b* b + c * c);
}

void matrix_camera_look_at(float* out, Vec3* eye, Vec3* center, Vec3* up) {
    float x0, x1, x2, y0, y1, y2, z0, z1, z2, len;

    z0 = eye->x - center->x;
    z1 = eye->y - center->y;
    z2 = eye->z - center->z;

    len = 1.0f / hypot3(z0, z1, z2);

    z0 *= len;
    z1 *= len;
    z2 *= len;

    x0 = up->y * z2 - up->z * z1;
    x1 = up->z * z0 - up->x * z2;
    x2 = up->x * z1 - up->y * z0;

    len = hypot3(x0, x1, x2);

    if (len == 0.0f) {
        x0 = 0;
        x1 = 0;
        x2 = 0;
    } else {
        len = 1.0f / len;

        x0 *= len;
        x1 *= len;
        x2 *= len;
    }

    y0 = z1 * x2 - z2 * x1;
    y1 = z2 * x0 - z0 * x2;
    y2 = z0 * x1 - z1 * x0;

    len = hypot3(y0, y1, y2);

    if (len == 0.0f) {
        y0 = 0;
        y1 = 0;
        y2 = 0;
    } else {
        len = 1.0f / len;

        y0 *= len;
        y1 *= len;
        y2 *= len;
    }

    out[0] = x0;
    out[1] = y0;
    out[2] = z0;
    out[3] = 0;
    out[4] = x1;
    out[5] = y1;
    out[6] = z1;
    out[7] = 0;
    out[8] = x2;
    out[9] = y2;
    out[10] = z2;
    out[11] = 0;
    out[12] = -(x0 * eye->x + x1 * eye->y + x2 * eye->z);
    out[13] = -(y0 * eye->x + y1 * eye->y + y2 * eye->z);
    out[14] = -(z0 * eye->x + z1 * eye->y + z2 * eye->z);
    out[15] = 1;
}

void matrix_get_scaling(float* mat, Vec3* out) {
    float m11 = mat[0];
    float m12 = mat[1];
    float m13 = mat[2];
    float m21 = mat[4];
    float m22 = mat[5];
    float m23 = mat[6];
    float m31 = mat[8];
    float m32 = mat[9];
    float m33 = mat[10];

    out->x = hypot3(m11, m12, m13);
    out->y = hypot3(m21, m22, m23);
    out->z = hypot3(m31, m32, m33);
}

void matrix_get_rotation(float* mat, Quat* out) {
    Vec3 scaling;
    matrix_get_scaling(mat, &scaling);

    float is1 = 1.0f / scaling.x;
    float is2 = 1.0f / scaling.x;
    float is3 = 1.0f / scaling.x;

    float sm11 = mat[0] * is1;
    float sm12 = mat[1] * is2;
    float sm13 = mat[2] * is3;
    float sm21 = mat[4] * is1;
    float sm22 = mat[5] * is2;
    float sm23 = mat[6] * is3;
    float sm31 = mat[8] * is1;
    float sm32 = mat[9] * is2;
    float sm33 = mat[10] * is3;

    float trace = sm11 + sm22 + sm33;

    float S = 0.0f;

    if (trace > 0) {
        S = fw64_sqrtf(trace + 1.0f) * 2;

        out->w = 0.25 * S;
        out->x = (sm23 - sm32) / S;
        out->y = (sm31 - sm13) / S;
        out->z = (sm12 - sm21) / S;
    } else if (sm11 > sm22 && sm11 > sm33) {
        S = fw64_sqrtf(1.0f + sm11 - sm22 - sm33) * 2;

        out->w = (sm23 - sm32) / S;
        out->x = 0.25 * S;
        out->y = (sm12 + sm21) / S;
        out->z = (sm31 + sm13) / S;
    } else if (sm22 > sm33) {
        S =  fw64_sqrtf(1.0f + sm22 - sm11 - sm33) * 2;

        out->w = (sm31 - sm13) / S;
        out->x = (sm12 + sm21) / S;
        out->y = 0.25f * S;
        out->z = (sm23 + sm32) / S;
    } else {
        S = fw64_sqrtf(1.0f + sm33 - sm11 - sm22) * 2;

        out->w = (sm12 - sm21) / S;
        out->x = (sm31 + sm13) / S;
        out->y = (sm23 + sm32) / S;
        out->z = 0.25f * S;
    }
}

void mat2_set_rotation(float* mat, float rad) {
    float s = fw64_sinf(rad);
    float c = fw64_cosf(rad);

    mat[0] = c;
    mat[1] = s;
    mat[2] = -s;
    mat[3] = c;
}

void mat2_transform_vec2(const float* mat, Vec2* vec) {
    Vec2 in = * vec;

    vec->x = in.x * mat[0] + in.y * mat[2];
    vec->y = in.x * mat[1] + in.y * mat[3];
}
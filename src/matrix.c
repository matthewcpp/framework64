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

void matrix_translate(float* matrix, Vec3* t) {
    matrix_set_identity(matrix);

    matrix[12] = t->x;
    matrix[13] = t->y;
    matrix[14] = t->z;
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

void matrix_from_quat(float* out, Quat* q) {
  float x2 = q->x + q->x;
  float y2 = q->y + q->y;
  float z2 = q->z + q->z;

  float xx = q->x * x2;
  float yx = q->y * x2;
  float yy = q->y * y2;
  float zx = q->z * x2;
  float zy = q->z * y2;
  float zz = q->z * z2;
  float wx = q->w * x2;
  float wy = q->w * y2;
  float wz = q->w * z2;

  out[0] = 1 - yy - zz;
  out[1] = yx + wz;
  out[2] = zx - wy;
  out[3] = 0;

  out[4] = yx - wz;
  out[5] = 1 - xx - zz;
  out[6] = zy + wx;
  out[7] = 0;

  out[8] = zx + wy;
  out[9] = zy - wx;
  out[10] = 1 - xx - yy;
  out[11] = 0;

  out[12] = 0;
  out[13] = 0;
  out[14] = 0;
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
    float a00 = a[0],
            a01 = a[1],
            a02 = a[2],
            a03 = a[3];

    float a10 = a[4],
            a11 = a[5],
            a12 = a[6],
            a13 = a[7];

    float a20 = a[8],
            a21 = a[9],
            a22 = a[10],
            a23 = a[11];

    float a30 = a[12],
            a31 = a[13],
            a32 = a[14],
            a33 = a[15];

    // Cache only the current line of the second matrix

    float b0 = b[0],
            b1 = b[1],
            b2 = b[2],
            b3 = b[3];

    out[0] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out[1] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out[2] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out[3] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;

    b0 = b[4];
    b1 = b[5];
    b2 = b[6];
    b3 = b[7];

    out[4] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out[5] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out[6] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out[7] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;

    b0 = b[8];
    b1 = b[9];
    b2 = b[10];
    b3 = b[11];

    out[8] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out[9] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out[10] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out[11] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;

    b0 = b[12];
    b1 = b[13];
    b2 = b[14];
    b3 = b[15];

    out[12] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
    out[13] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
    out[14] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
    out[15] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
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

void matrix_ortho2d(float* out, float left, float right, float bottom, float top) {
    matrix_ortho(out, left, right, bottom, top, -1.0f, 1.0f);
}

void matrix_ortho(float* out, float left, float right, float bottom, float top, float near, float far) {
    float lr = 1.0f / (left - right);
    float bt = 1.0f / (bottom - top);
    float nf = 1.0f / (near - far);

    out[0] = -2 * lr;
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = -2 * bt;
    out[6] = 0;
    out[7] = 0;
    out[8] = 0;
    out[9] = 0;
    out[10] = 2 * nf;
    out[11] = 0;
    out[12] = (left + right) * lr;
    out[13] = (top + bottom) * bt;
    out[14] = (far + near) * nf;
    out[15] = 1;
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
        x0 = 0.0f;
        x1 = 0.0f;
        x2 = 0.0f;
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
        y0 = 0.0f;
        y1 = 0.0f;
        y2 = 0.0f;
    } else {
        len = 1.0f / len;

        y0 *= len;
        y1 *= len;
        y2 *= len;
    }

    out[0] = x0;
    out[1] = y0;
    out[2] = z0;
    out[3] = 0.0f;
    out[4] = x1;
    out[5] = y1;
    out[6] = z1;
    out[7] = 0.0f;
    out[8] = x2;
    out[9] = y2;
    out[10] = z2;
    out[11] = 0.0f;
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

        out->w = 0.25f * S;
        out->x = (sm23 - sm32) / S;
        out->y = (sm31 - sm13) / S;
        out->z = (sm12 - sm21) / S;
    } else if (sm11 > sm22 && sm11 > sm33) {
        S = fw64_sqrtf(1.0f + sm11 - sm22 - sm33) * 2;

        out->w = (sm23 - sm32) / S;
        out->x = 0.25f * S;
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

void matrix_transpose(float* matrix) {
    float a01 = matrix[1];
    float a02 = matrix[2];
    float a03 = matrix[3];
    float a12 = matrix[6];
    float a13 = matrix[7];
    float a23 = matrix[11];

    matrix[1] = matrix[4];
    matrix[2] = matrix[8];
    matrix[3] = matrix[12];
    matrix[4] = a01;
    matrix[6] = matrix[9];
    matrix[7] = matrix[13];
    matrix[8] = a02;
    matrix[9] = a12;
    matrix[11] = matrix[14];
    matrix[12] = a03;
    matrix[13] = a13;
    matrix[14] = a23;
}

int matrix_invert(float* out, float* a) {
    float a00 = a[0],
    a01 = a[1],
    a02 = a[2],
    a03 = a[3];

    float a10 = a[4],
    a11 = a[5],
    a12 = a[6],
    a13 = a[7];

    float a20 = a[8],
    a21 = a[9],
    a22 = a[10],
    a23 = a[11];

    float a30 = a[12],
    a31 = a[13],
    a32 = a[14],
    a33 = a[15];

    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;

    // Calculate the determinant

    float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

    if (!det) {
        return 0;
    }

    det = 1.0f / det;

    out[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
    out[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
    out[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
    out[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
    out[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
    out[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
    out[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
    out[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
    out[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
    out[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
    out[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
    out[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
    out[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
    out[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
    out[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
    out[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;

    return 1;
}

void matrix_transform_vec3(const float* m, Vec3* vec) {
    float x = vec->x;
    float y = vec->y;
    float z = vec->z;

    float w = m[3] * x + m[7] * y + m[11] * z + m[15];
    if (w == 0.0f)
        w = 1.0f;

    vec->x = (m[0] * x + m[4] * y + m[8] * z + m[12]) / w;
    vec->y = (m[1] * x + m[5] * y + m[9] * z + m[13]) / w;
    vec->z = (m[2] * x + m[6] * y + m[10] * z + m[14]) / w;
}

void matrix_transform_vec4(const float* m, Vec4* vec) {
    float x = vec->x;
    float y = vec->y;
    float z = vec->z;
    float w = vec->w;

    vec->x = m[0] * x + m[4] * y + m[8] * z + m[12] * w;
    vec->y = m[1] * x + m[5] * y + m[9] * z + m[13] * w;
    vec->z = m[2] * x + m[6] * y + m[10] * z + m[14] * w;
    vec->w = m[3] * x + m[7] * y + m[11] * z + m[15] * w;
}

void mat3_set_from_mat4(float* out, const float* a) {
    out[0] = a[0];
    out[1] = a[1];
    out[2] = a[2];

    out[3] = a[4];
    out[4] = a[5];
    out[5] = a[6];

    out[6] = a[8];
    out[7] = a[9];
    out[8] = a[10];
}

void mat3_transform_vec3(const float* m, Vec3* v) {
    float x = v->x;
    float y = v->y;
    float z = v->z;

    v->x = x * m[0] + y * m[3] + z * m[6];
    v->y = x * m[1] + y * m[4] + z * m[7];
    v->z = x * m[2] + y * m[5] + z * m[8];
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

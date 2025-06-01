#include "framework64/matrix_project.h"

#include "framework64/matrix.h"
#include "framework64/vec4.h"

int fw64_matrix_project(Vec3* pt, float* modelview, float* projection, IVec2* view_size, Vec3* result) {
    Vec4 pt4;
    vec4_set(&pt4, pt->x, pt->y, pt->z, 1.0f);

    matrix_transform_vec4(modelview, &pt4);
    matrix_transform_vec4(projection, &pt4);

    if (pt4.w == 0.0f) {
        return 0;
    }

    vec4_div(&pt4, pt4.w, &pt4);

    pt4.x = (pt4.x + 1.0f) / 2.0f;
    pt4.y = (pt4.y + 1.0f) / 2.0f;
    pt4.z = (pt4.z + 1.0f) / 2.0f;

    /* Map x,y to viewport */
    pt4.x = pt4.x * view_size->x;
    pt4.y = pt4.y * view_size->y;

    result->x = pt4.x;
    result->y = pt4.y;
    result->z = pt4.z;

    return 1;
}

int fw64_matrix_unproject(Vec3* pt, float* modelview, float* projection, IVec2* view_pos, IVec2* view_size, Vec3* result) {
    float inverse[16];
    matrix_multiply(projection, modelview, inverse);

    if (!matrix_invert(inverse, inverse)) {
        return 0;
    }

    Vec4 pt4;
    // Transformation of normalized coordinates between -1 and 1
    pt4.x = (pt->x - (float)view_pos->x)/(float)view_size->x * 2.0f - 1.0f;
    pt4.y = (pt->y - (float)view_pos->y)/(float)view_size->y * 2.0f - 1.0f;
    pt4.z = 2.0f * pt->z - 1.0f;
    pt4.w = 1.0f;

    matrix_transform_vec4(inverse, &pt4);
    vec4_div(&pt4, pt4.w, &pt4);

    result->x = pt4.x;
    result->y = pt4.y;
    result->z = pt4.z;

    return 1;
}

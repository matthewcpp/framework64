#include "framework64/transform.h"
#include "framework64/matrix.h"

void fw64_transform_init(fw64Transform* transform) {
    vec3_zero(&transform->position);
    quat_ident(&transform->rotation);
    vec3_one(&transform->scale);
    guMtxIdent(&transform->matrix);
}

void fw64_transform_forward(fw64Transform* transform, Vec3* out) {
    Vec3 forward = {0.0f, 0.0f, -1.0f};
    quat_transform_vec3(out, &transform->rotation, &forward);
    vec3_normalize(out);
}

void fw64_transform_back(fw64Transform* transform, Vec3* out) {
    fw64_transform_forward(transform, out);
    vec3_negate(out);
}

void fw64_transform_up(fw64Transform* transform, Vec3* out) {
    Vec3 up = {0.0f, 1.0f, 0.0f};
    quat_transform_vec3(out, &transform->rotation, &up);
    vec3_normalize(out);
}

void fw64_transform_down(fw64Transform* transform, Vec3* out) {
    fw64_transform_up(transform, out);
    vec3_negate(out);
}

void fw64_transform_right(fw64Transform* transform, Vec3* out) {
    Vec3 right = {1.0f, 0.0f, 0.0f};
    quat_transform_vec3(out, &transform->rotation, &right);
    vec3_normalize(out);
}

void fw64_transform_left(fw64Transform* transform, Vec3* out) {
    fw64_transform_right(transform, out);
    vec3_negate(out);
}

void fw64_transform_look_at(fw64Transform* transform, Vec3* target, Vec3* up) {
    float matrix[16];
    matrix_target_to(&matrix[0], &transform->position, target, up);

    matrix_get_rotation(matrix, &transform->rotation);
    quat_normalize( &transform->rotation);
}

void fw64_transform_update_matrix(fw64Transform* transform) {
    float fmatrix[16];
    matrix_from_trs(fmatrix, &transform->position, &transform->rotation, &transform->scale);

    guMtxF2L((float (*)[4])fmatrix, &transform->matrix);
}
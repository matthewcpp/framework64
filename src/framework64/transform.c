#include "framework64/transform.h"
#include "framework64/matrix.h"

void fw64_transform_init(fw64Transform* transform) {
    vec3_set_zero(&transform->position);
    quat_ident(&transform->rotation);
    vec3_set_one(&transform->scale);
    matrix_set_identity(transform->world_matrix);
}

void fw64_transform_forward(const fw64Transform* transform, Vec3* out) {
    Vec3 forward =  vec3_forward();
    quat_transform_vec3(&transform->rotation, &forward, out);
    vec3_normalize(out);
}

void fw64_transform_back(const fw64Transform* transform, Vec3* out) {
    fw64_transform_forward(transform, out);
    vec3_negate(out);
}

void fw64_transform_up(const fw64Transform* transform, Vec3* out) {
    Vec3 up = vec3_up();
    quat_transform_vec3(&transform->rotation, &up, out);
    vec3_normalize(out);
}

void fw64_transform_down(const fw64Transform* transform, Vec3* out) {
    fw64_transform_up(transform, out);
    vec3_negate(out);
}

void fw64_transform_right(const fw64Transform* transform, Vec3* out) {
    Vec3 right = vec3_right();
    quat_transform_vec3(&transform->rotation, &right, out);
    vec3_normalize(out);
}

void fw64_transform_left(const fw64Transform* transform, Vec3* out) {
    fw64_transform_right(transform, out);
    vec3_negate(out);
}

// TODO?: https://community.khronos.org/t/implement-unity-transform-lookat-function/72397
void fw64_transform_look_at(fw64Transform* transform, const Vec3* target, const Vec3* up) {
    float matrix[16];
    matrix_target_to(&matrix[0], &transform->position, target, up);

    matrix_get_rotation(matrix, &transform->rotation);
    quat_normalize( &transform->rotation);
}

void fw64_transform_update_matrix(fw64Transform* transform) {
    matrix_from_trs(transform->world_matrix, &transform->position, &transform->rotation, &transform->scale);
}

void fw64_transform_inv_mult_point(const fw64Transform* transform, const Vec3* point, Vec3* out) {
    Quat inv_rot = transform->rotation;
    quat_conjugate(&inv_rot);

    vec3_subtract(point, &transform->position, out);
    quat_transform_vec3(&inv_rot, out, out);
}

void fw64_transform_mult_point(const fw64Transform* transform, const Vec3* point, Vec3* out) {
    quat_transform_vec3(&transform->rotation, point, out);
    vec3_add(&transform->position, out, out);
}

void fw64_transform_xform_box(const fw64Transform* transform, const Box* source, Box* target) {
     matrix_transform_box(transform->world_matrix, source, target);
}

#include "framework64/transform.h"
#include "framework64/matrix.h"

void transform_init(Transform* transform) {
    vec3_zero(&transform->position);
    quat_ident(&transform->rotation);
    vec3_one(&transform->scale);
}

void transform_forward(Transform* transform, Vec3* out) {
    Vec3 forward = {0.0f, 0.0f, -1.0f};
    quat_transform_vec3(out, &transform->rotation, &forward);
}

void transform_up(Transform* transform, Vec3* out) {
    Vec3 up = {0.0f, 1.0f, 0.0f};
    quat_transform_vec3(out, &transform->rotation, &up);
}

void transform_look_at(Transform* transform, Vec3* target, Vec3* up) {
    float matrix[16];
    matrix_target_to(&matrix[0], &transform->position, target, up);

    matrix_get_rotation(matrix, &transform->rotation);
    quat_normalize( &transform->rotation);
}
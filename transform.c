#include "transform.h"

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
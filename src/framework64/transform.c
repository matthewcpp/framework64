#include "framework64/transform.h"
#include "framework64/matrix.h"

void fw64_transform_init(fw64Transform* transform) {
    fw64_transform_reset(transform);
    transform->parent = NULL;
    transform->first_child = NULL;
    transform->next_sibling = NULL;
    matrix_set_identity(transform->world_matrix);
}

void fw64_transform_reset(fw64Transform* transform) {
    vec3_set_zero(&transform->position);
    quat_ident(&transform->rotation);
    vec3_set_one(&transform->scale);
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
    if (transform->parent) {
        float local_matrix[16];
        matrix_from_trs(local_matrix, &transform->position, &transform->rotation, &transform->scale);
        matrix_multiply(transform->parent->world_matrix, local_matrix, transform->world_matrix);
    } else {
        matrix_from_trs(transform->world_matrix, &transform->position, &transform->rotation, &transform->scale);
    }

    fw64Transform* child = transform->first_child;
    while (child) {
        fw64_transform_update_matrix(child);
        child = child->next_sibling;
    }
}

void fw64_transform_get_world_position(fw64Transform* transform, Vec3* out) {
    out->x = transform->world_matrix[12];
    out->x = transform->world_matrix[13];
    out->x = transform->world_matrix[14];
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

void fw64_transform_add_child(fw64Transform* parent, fw64Transform* child) {
    if (child->parent) {
        fw64_transform_remove_child(child->parent, child);
    }

    if (parent->first_child) {
        child->next_sibling = parent->first_child;
    } else {
        child->next_sibling = NULL;
    }

    parent->first_child = child;
    child->parent = parent;
}

int fw64_transform_remove_child(fw64Transform* parent, fw64Transform* child) {
    // are we removing first child?
    if (parent->first_child == child) {
        parent->first_child = child->next_sibling;
        child->next_sibling = NULL;
        child->parent = NULL;
        return 1;
    }

    fw64Transform* prev_child = parent->first_child ;
    fw64Transform* current_child = parent->first_child->next_sibling;

    while (current_child) {
        if (current_child == child) {
            prev_child->next_sibling = current_child->next_sibling;
            child->next_sibling = NULL;
            child->parent = NULL;
            return 1;
        }

        prev_child = current_child;
        current_child = current_child->next_sibling;
    }
    
    return 0;
}

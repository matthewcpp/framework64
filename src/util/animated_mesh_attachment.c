#include "framework64/util/animated_mesh_attachment.h"

void fw64_animated_mesh_attachment_init(fw64AnimatedMeshAttachment* attachment, fw64Transform* reference, fw64AnimationController* controller, uint32_t joint_index) {
    attachment->reference = reference;
    attachment->controller = controller;
    attachment->joint_index = joint_index;

    vec3_zero(&attachment->local_position);
    quat_ident(&attachment->local_rotation);
    vec3_set_all(&attachment->local_scale, 1.0f);
}


void fw64_animated_mesh_attachment_update(fw64AnimatedMeshAttachment* attachment) {
    float reference_matrix[16];
    float final_matrix[16];
    float local_matrix[16];
    float attach_matrix[16];

    float* world_matrix = fw64_animation_controller_get_world_matrix(attachment->controller, attachment->joint_index);

    matrix_from_trs(&reference_matrix[0], &attachment->reference->position, &attachment->reference->rotation, &attachment->reference->scale);
    matrix_from_trs(&local_matrix[0], &attachment->local_position, &attachment->local_rotation, &attachment->local_scale);

    matrix_multiply(&attach_matrix[0], world_matrix, local_matrix);
    matrix_multiply(&final_matrix[0], &reference_matrix[0], attach_matrix);

    fw64_matrix_set_from_array(&attachment->matrix, &final_matrix[0]);
}
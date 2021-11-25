#include "framework64/animation_controller.h"

#include "framework64/matrix.h"
#include "framework64/math.h"

#include <string.h>

#define VEC_KEYFRAME(buffer, index, count) ((buffer) + ((index) * (count)))
#define VEC3_KEYFRAME(buffer, index) (Vec3*)VEC_KEYFRAME(buffer, index, 3)
#define QUAT_KEYFRAME(buffer, index) (Quat*)VEC_KEYFRAME(buffer, index, 4)
#define FLOAT_MATRIX(matrices, index) ((matrices) + ((index) * 16));

static void initialize_matrices(fw64AnimationController* controller);
static void update_skin(fw64AnimationController* controller);
static void update_joint(fw64AnimationController* controller, uint32_t joint_index, float* parent_matirx);
static void compute_local_matrix(fw64AnimationController* controller, uint32_t joint_index, float* local_matrix);
static uint32_t get_keyframe_index(float* input_buffer, uint32_t key_count, float time);

void fw64_animation_controller_init(fw64AnimationController* controller, fw64AnimationData* animation_data, int initial_animation, fw64Allocator* allocator) {
    if (allocator == NULL) allocator = fw64_default_allocator();

    controller->animation_data = animation_data;
    controller->matrices = allocator->memalign(allocator, 8, sizeof(fw64Matrix) * animation_data->skin.joint_count);
    controller->speed = 1.0f;
    controller->loop = 1;
    controller->state = FW64_ANIMATION_STATE_STOPPED;

    initialize_matrices(controller);

    if (initial_animation >= 0)
        fw64_animation_controller_set_animation(controller, initial_animation);
    else
        controller->current_animation = NULL;
}

void fw64_animation_controller_uninit(fw64AnimationController* controller, fw64Allocator* allocator) {
    allocator->free(allocator, controller->matrices);
}

void fw64_animation_controller_play(fw64AnimationController* controller) {
    controller->state = FW64_ANIMATION_STATE_PLAYING;
}

void fw64_animation_controller_pause(fw64AnimationController* controller) {
    controller->state = FW64_ANIMATION_STATE_PAUSED;
}

void fw64_animation_controller_stop(fw64AnimationController* controller) {
    controller->state = FW64_ANIMATION_STATE_STOPPED;
    fw64_animation_controller_set_time(controller, 0.0f);
}

void fw64_animation_controller_set_animation(fw64AnimationController* controller, int index) {
    controller->current_animation = controller->animation_data->animations + index;
    fw64_animation_controller_set_time(controller, 0.0f);
}

void fw64_animation_controller_update(fw64AnimationController* controller, float time_delta) {
    if (controller->state != FW64_ANIMATION_STATE_PLAYING)
        return;

    float next_time = controller->current_time + (time_delta * controller->speed);

    if (controller->loop) {
        while (next_time < 0.0f)
            next_time += controller->current_animation->total_time;

        while (next_time > controller->current_animation->total_time)
            next_time -= controller->current_animation->total_time;
    }
    else {
        next_time = fw64_clamp(next_time, 0.0f, controller->current_animation->total_time);
    }

    controller->current_time = next_time;
    update_skin(controller);
}

void fw64_animation_controller_set_time(fw64AnimationController* controller, float time) {
    controller->current_time = fw64_clamp(time, 0.0f, controller->current_animation->total_time);
    update_skin(controller);
}

void update_skin(fw64AnimationController* controller) {
    fw64AnimationJoint* root_joint = controller->animation_data->skin.joints;
    uint16_t* root_children = controller->animation_data->skin.hierarchy + root_joint->children_index;


    fw64Matrix* root_matrix = controller->matrices;
    float local_matrix[16];

    #ifdef PLATFORM_N64
        guMtxL2F((float (*)[4])local_matrix, root_matrix);
    #else
        memcpy(local_matrix, &root_matrix->m[0], sizeof(float) * 16);
    #endif
    
    for (uint16_t i = 0; i < root_joint->children_count; i++) {
        update_joint(controller, root_children[i], &local_matrix[0]);
    }
}

uint32_t get_keyframe_index(float* input_buffer, uint32_t key_count, float time) {
    for (uint32_t i = 0; i < key_count; i++) {
        if (time <= input_buffer[i])
            return i;
    }

    return key_count;
}

#define VEC_KEYFRAME(buffer, index, count) ((buffer) + ((index) * (count)))
#define VEC3_KEYFRAME(buffer, index) (Vec3*)VEC_KEYFRAME(buffer, index, 3)
#define QUAT_KEYFRAME(buffer, index) (Quat*)VEC_KEYFRAME(buffer, index, 4)
#define FLOAT_MATRIX(matrices, index) ((matrices) + ((index) * 16));

void compute_local_matrix(fw64AnimationController* controller, uint32_t joint_index, float* local_matrix) {
    float* keyframe_data = controller->animation_data->keyframe_data;
    fw64AnimationTrack* track = controller->animation_data->tracks + (controller->current_animation->tracks_index + joint_index);

    float* input_buffer = keyframe_data + controller->current_animation->input_index;
    float* translation_buffer = keyframe_data + track->translation;
    float* rotation_buffer = keyframe_data + track->rotation;
    float* scale_buffer = keyframe_data + track->scale;

    uint32_t keyframe_index = get_keyframe_index(input_buffer, controller->current_animation->key_count, controller->current_time);

    if (keyframe_index == 0) {
        matrix_from_trs(&local_matrix[0],
                        VEC3_KEYFRAME(translation_buffer, 0),
                        QUAT_KEYFRAME(rotation_buffer, 0),
                        VEC3_KEYFRAME(scale_buffer, 0));
    }
    else if (keyframe_index == controller->current_animation->key_count) {
        matrix_from_trs(&local_matrix[0],
                        VEC3_KEYFRAME(translation_buffer, keyframe_index - 1),
                        QUAT_KEYFRAME(rotation_buffer, keyframe_index - 1),
                        VEC3_KEYFRAME(scale_buffer, keyframe_index - 1));
    }
    else {
        uint32_t a = keyframe_index - 1;
        uint32_t b = keyframe_index;

        Vec3 translation, scale;
        Quat rotation;
        float t = (controller->current_time - input_buffer[a]) / (input_buffer[b] - input_buffer[a]);

        vec3_lerp(&translation, VEC3_KEYFRAME(translation_buffer, a), VEC3_KEYFRAME(translation_buffer, b), t);
        quat_slerp(&rotation, QUAT_KEYFRAME(rotation_buffer, a), QUAT_KEYFRAME(rotation_buffer, b), t);
        vec3_lerp(&scale, VEC3_KEYFRAME(scale_buffer, a), VEC3_KEYFRAME(scale_buffer, b), t);

        matrix_from_trs(&local_matrix[0], &translation, &rotation, &scale);
    }
}

void update_joint(fw64AnimationController* controller, uint32_t joint_index, float* parent_matrix) {
    fw64AnimationJoint* joint = controller->animation_data->skin.joints + joint_index;
    uint16_t* joint_children = controller->animation_data->skin.hierarchy + joint->children_index;

    float local_matrix[16];
    float world_matrix[16];
    float* inv_bind_matrix = FLOAT_MATRIX(controller->animation_data->skin.inverse_bind_matrices, joint_index);

    compute_local_matrix(controller, joint_index, &local_matrix[0]);
    matrix_multiply(&world_matrix[0], parent_matrix, &local_matrix[0] );

    fw64Matrix* joint_matrix = controller->matrices + joint_index;
    

#ifdef PLATFORM_N64
    float temp[16];
    matrix_multiply(&temp[0], &world_matrix[0], inv_bind_matrix);
    guMtxF2L((float (*)[4])temp, joint_matrix);
#else
    matrix_multiply(&joint_matrix->m[0], &world_matrix[0], inv_bind_matrix);
#endif

    for (uint16_t i = 0; i < joint->children_count; i++) {
        update_joint(controller, joint_children[i], &world_matrix[0]);
    }
}

#ifdef PLATFORM_N64
#include <nusys.h>

void initialize_matrices(fw64AnimationController* controller) {
    for (uint32_t i = 0; i < controller->animation_data->skin.joint_count; i++) {
        guMtxIdent(controller->matrices + i);
    }
}

#else

void initialize_matrices(fw64AnimationController* controller) {
    for (uint32_t i = 0; i < controller->animation_data->skin.joint_count; i++) {
        matrix_set_identity(&controller->matrices[i].m[0]);
    }
}

#endif

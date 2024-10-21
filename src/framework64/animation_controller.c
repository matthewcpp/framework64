#include "framework64/animation_controller.h"

#include "framework64/matrix.h"
#include "framework64/math.h"

#include <string.h>

#define VEC_KEYFRAME(buffer, index, count) ((buffer) + ((index) * (count)))
#define VEC3_KEYFRAME(buffer, index) (Vec3*)VEC_KEYFRAME(buffer, index, 3)
#define QUAT_KEYFRAME(buffer, index) (Quat*)VEC_KEYFRAME(buffer, index, 4)
#define FLOAT_MATRIX(matrices, index) ((matrices) + ((index) * 16));

typedef struct {
    uint32_t count;
    float times[];
} fw64AnimationInputBuffer;

/** This struct acts as a convience way to access input buffers */
typedef struct {
    uint32_t count;
    float values[];
} fw64AnimationValueBuffer;

#define INVALID_KEYFRAME_BUFFER_INDEX UINT16_MAX

static void initialize_matrices(fw64AnimationController* controller);
static void update_skin(fw64AnimationController* controller);
static void update_joint(fw64AnimationController* controller, uint32_t joint_index, float* parent_matirx);
static void compute_local_matrix(fw64AnimationController* controller, uint32_t joint_index, float* local_matrix);
static uint32_t get_keyframe_index(float* input_buffer, uint32_t key_count, float time);

void fw64_animation_controller_init(fw64AnimationController* controller, fw64AnimationData* animation_data, int initial_animation, fw64Allocator* allocator) {
    controller->animation_data = animation_data;
    controller->matrices = fw64_allocator_memalign(allocator, 8, sizeof(fw64Matrix) * animation_data->skin.joint_count);
    controller->speed = 1.0f;
    controller->loop = 1;
    controller->state = FW64_ANIMATION_STATE_STOPPED;

    initialize_matrices(controller);

    if (initial_animation >= 0) {
        fw64_animation_controller_set_animation(controller, initial_animation);
    }
    else {
        controller->current_animation = NULL;
    }
}

void fw64_animation_controller_uninit(fw64AnimationController* controller, fw64Allocator* allocator) {
    fw64_allocator_free(allocator, controller->matrices);
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
    if (controller->state != FW64_ANIMATION_STATE_PLAYING) {
        return;
    }

    float next_time = controller->current_time + (time_delta * controller->speed);

    if (controller->loop) {
        while (next_time < 0.0f) {
            next_time += controller->current_animation->total_time;
        }

        while (next_time > controller->current_animation->total_time) {
            next_time -= controller->current_animation->total_time;
        }
    }
    else {
        next_time = fw64_clamp(next_time, 0.0f, controller->current_animation->total_time);

        if (next_time >= controller->current_animation->total_time) { // did we just reach the end of the animation?
            controller->state = FW64_ANIMATION_STATE_STOPPED;
        }
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
    fw64AnimationTrack* root_track = controller->animation_data->tracks + (controller->current_animation->tracks_index);


    fw64Matrix* root_matrix = controller->matrices;
    float local_matrix[16];
    matrix_set_identity(&local_matrix[0]);

    // in this case the Armature itself has animation applied to it.
    // we need to compute the TRS for this node and set it as the root matrix
    // in either case this matrix is fed into the function which calcualtes child joint matrices
    if (root_track->translation != INVALID_KEYFRAME_BUFFER_INDEX) {
        compute_local_matrix(controller, 0, &local_matrix[0]);
    }

    #ifdef FW64_PLATFORM_N64_LIBULTRA
        guMtxF2L((float (*)[4])local_matrix, root_matrix);
    #else
        memcpy(&root_matrix->m[0], &local_matrix[0], sizeof(float) * 16);
    #endif
    
    for (uint16_t i = 0; i < root_joint->children_count; i++) {
        update_joint(controller, root_children[i], &local_matrix[0]);
    }
}

uint32_t get_keyframe_index(float* input_buffer, uint32_t key_count, float time) {
    for (uint32_t i = 0; i < key_count; i++) {
        if (time <= input_buffer[i]) {
            return i;
        }
    }

    return key_count;
}

#define VEC_KEYFRAME(buffer, index, count) ((buffer) + ((index) * (count)))
#define VEC3_KEYFRAME(buffer, index) (Vec3*)VEC_KEYFRAME(buffer, index, 3)
#define QUAT_KEYFRAME(buffer, index) (Quat*)VEC_KEYFRAME(buffer, index, 4)
#define FLOAT_MATRIX(matrices, index) ((matrices) + ((index) * 16));

static void compute_vec3_value(fw64AnimationController* controller, uint16_t input_index, uint16_t value_index, Vec3* value) {
    float* keyframe_data = controller->animation_data->keyframe_data;

    fw64AnimationInputBuffer* input_buffer = (fw64AnimationInputBuffer*)(keyframe_data + input_index);
    fw64AnimationValueBuffer* value_buffer = (fw64AnimationValueBuffer*)(keyframe_data + value_index);
    uint32_t key_count = input_buffer->count;

    uint32_t keyframe_index = get_keyframe_index(input_buffer->times, key_count, controller->current_time);

    if (keyframe_index == 0) {
        memcpy(value, VEC3_KEYFRAME(value_buffer->values, 0), sizeof(Vec3));
    } else if (keyframe_index == key_count) {
        memcpy(value, VEC3_KEYFRAME(value_buffer->values, keyframe_index - 1), sizeof(Vec3));
    } else {
        uint32_t a = keyframe_index - 1;
        uint32_t b = keyframe_index;

        float t = (controller->current_time - input_buffer->times[a]) / (input_buffer->times[b] - input_buffer->times[a]);
        vec3_lerp(value, VEC3_KEYFRAME(value_buffer->values, a), VEC3_KEYFRAME(value_buffer->values, b), t);
    }
}

static void compute_quat_value(fw64AnimationController* controller, uint16_t input_index, uint16_t value_index, Quat* value) {
    float* keyframe_data = controller->animation_data->keyframe_data;

    fw64AnimationInputBuffer* input_buffer = (fw64AnimationInputBuffer*)(keyframe_data + input_index);
    fw64AnimationValueBuffer* value_buffer = (fw64AnimationValueBuffer*)(keyframe_data + value_index);
    uint32_t key_count = input_buffer->count;

    uint32_t keyframe_index = get_keyframe_index(input_buffer->times, key_count, controller->current_time);

    if (keyframe_index == 0) {
        memcpy(value, QUAT_KEYFRAME(value_buffer->values, 0), sizeof(Quat));
    } else if (keyframe_index == key_count) {
        memcpy(value, QUAT_KEYFRAME(value_buffer->values, keyframe_index - 1), sizeof(Quat));
    } else {
        uint32_t a = keyframe_index - 1;
        uint32_t b = keyframe_index;

        float t = (controller->current_time - input_buffer->times[a]) / (input_buffer->times[b] - input_buffer->times[a]);
        quat_slerp(value, QUAT_KEYFRAME(value_buffer->values, a), QUAT_KEYFRAME(value_buffer->values, b), t);
    }
}

void compute_local_matrix(fw64AnimationController* controller, uint32_t joint_index, float* local_matrix) {
    fw64AnimationTrack* track = controller->animation_data->tracks + (controller->current_animation->tracks_index + joint_index);

    Vec3 translation, scale;
    Quat rotation;

    compute_vec3_value(controller, track->translation_input_index, track->translation, &translation);
    compute_vec3_value(controller, track->scale_input_index, track->scale, &scale);
    compute_quat_value(controller, track->rotation_input_index, track->rotation, &rotation);

    matrix_from_trs(&local_matrix[0], &translation, &rotation, &scale);
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
    

#ifdef FW64_PLATFORM_N64_LIBULTRA
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

#ifdef FW64_PLATFORM_N64_LIBULTRA
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

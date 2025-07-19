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
    float times[1];
} fw64AnimationInputBuffer;

/** This struct acts as a convience way to access input buffers */
typedef struct {
    uint32_t count;
    float values[1];
} fw64AnimationValueBuffer;

#define INVALID_KEYFRAME_BUFFER_INDEX UINT16_MAX

static void update_skin(fw64AnimationController* controller);
static void update_hierarchy(fw64AnimationController* controller);
static void compute_joint_trs(fw64AnimationController* controller, uint32_t joint_index);
static uint32_t get_keyframe_index(float* input_buffer, uint32_t key_count, float time);

static void setup_joint_hierarchy(fw64AnimationController* controller, fw64Transform* joint_transform, const fw64AnimationJoint* joint_info, fw64Transform* parent) {
    fw64_transform_reset(joint_transform);
    joint_transform->parent = parent;
    const uint16_t* joint_transform_children = controller->animation_data->skin.hierarchy + joint_info->children_index;

    if (joint_info->children_count == 0) {
        joint_transform->first_child = NULL;
        return;
    }

    fw64Transform* prev_child_transform = controller->joint_transforms + joint_transform_children[0];
    fw64AnimationJoint* prev_child_joint_info = controller->animation_data->skin.joints + joint_transform_children[0];
    joint_transform->first_child = prev_child_transform;
    setup_joint_hierarchy(controller, prev_child_transform, prev_child_joint_info, joint_transform);

    for (uint32_t i = 1; i < joint_info->children_count; i++) {
        fw64Transform* current_child_transform = controller->joint_transforms + joint_transform_children[i];
        fw64AnimationJoint* current_child_joint_info = controller->animation_data->skin.joints + joint_transform_children[i];

        prev_child_transform->next_sibling = current_child_transform;
        setup_joint_hierarchy(controller, current_child_transform, current_child_joint_info, joint_transform);

        prev_child_transform = current_child_transform;
    }

    prev_child_transform->next_sibling = NULL;
}

void fw64_animation_controller_init(fw64AnimationController* controller, fw64AnimationData* animation_data, fw64AnimationId initial_animation, fw64Transform* parent_transform, fw64Allocator* allocator) {
    controller->animation_data = animation_data;
    controller->parent_transform = parent_transform;
    controller->joint_transforms = fw64_allocator_malloc(allocator, sizeof(fw64Transform) * animation_data->skin.joint_count);

    setup_joint_hierarchy(controller, controller->joint_transforms, animation_data->skin.joints, NULL);

    controller->speed = 1.0f;
    controller->loop = 1;
    controller->state = FW64_ANIMATION_STATE_STOPPED;
    controller->matrices = fw64_allocator_memalign(allocator, 8, sizeof(fw64Matrix) * animation_data->skin.joint_count);

#ifdef FW64_PLATFORM_N64_LIBULTRA
    // TODO: is this needed?
    for (uint32_t i = 0; i < controller->animation_data->skin.joint_count; i++) {
        guMtxIdent(controller->matrices + i);
    }
#endif

    if (initial_animation != FW64_INVALID_ANIMATION_ID) {
        fw64_animation_controller_set_animation(controller, initial_animation);
    }
    else {
        controller->current_animation = NULL;
    }
}

void fw64_animation_controller_uninit(fw64AnimationController* controller, fw64Allocator* allocator) {
    fw64_allocator_free(allocator, controller->joint_transforms);
    #ifdef FW64_PLATFORM_N64_LIBULTRA
    fw64_allocator_free(allocator, controller->matrices);
    #endif
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

void fw64_animation_controller_set_animation(fw64AnimationController* controller, fw64AnimationId index) {
    controller->current_animation = controller->animation_data->animations + index;
    fw64_animation_controller_set_time(controller, 0.0f);
}

void fw64_animation_controller_update(fw64AnimationController* controller, float time_delta) {
    if (controller->state != FW64_ANIMATION_STATE_PLAYING) {
        // If the animation is not playing, we still need to update the hierarchy in case the node we are attached to has moved
        // TODO: maybe add a paramter to control this?
        update_hierarchy(controller);
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

        // did we just reach the end of the animation?
        if (next_time >= controller->current_animation->total_time) {
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

void update_hierarchy(fw64AnimationController* controller) {
    fw64Transform* root_transform = controller->joint_transforms;
    fw64AnimationTrack* root_track = controller->animation_data->tracks + (controller->current_animation->tracks_index);

    // if the Armature itself has animation applied to it, we need to compute the TRS for this node and set it as the root matrix
    // otherwise we will just use the parent matrix for the node that this skeleton is attached to.
    if (root_track->translation != INVALID_KEYFRAME_BUFFER_INDEX) {
        float local_root_matrix[16];
        compute_joint_trs(controller, 0);
        matrix_from_trs(local_root_matrix, &root_transform->position, &root_transform->rotation, &root_transform->scale);
        matrix_multiply(controller->parent_transform->world_matrix, local_root_matrix, root_transform->world_matrix);
    } else {
        memcpy(root_transform->world_matrix, controller->parent_transform->world_matrix, sizeof(float) * 16);
    }

    // The skeleton root's world matrix is already set above. We need to kick off the recursive update manually due to
    // the fact that we do not have a physical connection to the node which this skeleton is attached.  Using
    // fw64_transform_update_matrix on the root transform would overwrite the matrix set above.
    fw64Transform* child = root_transform->first_child;
    while(child) {
        fw64_transform_update_matrix(child);
        child = child->next_sibling;
    }

    for (uint32_t joint_index = 0; joint_index < controller->animation_data->skin.joint_count; joint_index++) {
        fw64Transform* joint_transform = controller->joint_transforms + joint_index;
        fw64Matrix* joint_matrix = controller->matrices + joint_index;
        float* inv_bind_matrix = FLOAT_MATRIX(controller->animation_data->skin.inverse_bind_matrices, joint_index);

    #ifdef FW64_PLATFORM_N64_LIBULTRA
        float temp[16];
        matrix_multiply(joint_transform->world_matrix, inv_bind_matrix, &temp[0]);
        guMtxF2L((float (*)[4])temp, joint_matrix);
    #else
        matrix_multiply(joint_transform->world_matrix, inv_bind_matrix, &joint_matrix->m[0]);
    #endif
    }
}

void update_skin(fw64AnimationController* controller) {
    // updating the skeleton involves three phases:
    // 1. Computing the local TRS for each joint
    // 2. Updating the world matrices for each joint
    // 3. Computing the final matrix (world * inv_bind) for each joint
    for (uint32_t i = 1; i < controller->animation_data->skin.joint_count; i++) {
        compute_joint_trs(controller, i);
    }

    update_hierarchy(controller);
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
        vec3_lerp(VEC3_KEYFRAME(value_buffer->values, a), VEC3_KEYFRAME(value_buffer->values, b), t, value);
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
        quat_slerp(QUAT_KEYFRAME(value_buffer->values, a), QUAT_KEYFRAME(value_buffer->values, b), t, value);
    }
}

void compute_joint_trs(fw64AnimationController* controller, uint32_t joint_index) {
    fw64AnimationTrack* track = controller->animation_data->tracks + (controller->current_animation->tracks_index + joint_index);
    fw64Transform* joint_transform = controller->joint_transforms + joint_index;

    compute_vec3_value(controller, track->translation_input_index, track->translation, &joint_transform->position);
    compute_vec3_value(controller, track->scale_input_index, track->scale, &joint_transform->scale);
    compute_quat_value(controller, track->rotation_input_index, track->rotation, &joint_transform->rotation);
}

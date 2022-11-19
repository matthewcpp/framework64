#pragma once

/** \file animation_controller.h */

#include "animation_data.h"

#ifndef FW64_JOINT_ATTACHMENT_COUNT
#define FW64_JOINT_ATTACHMENT_COUNT 2
#endif

typedef enum {
    FW64_ANIMATION_STATE_STOPPED,
    FW64_ANIMATION_STATE_PLAYING,
    FW64_ANIMATION_STATE_PAUSED
} fw64AnimationState;

typedef struct {
    fw64Matrix final_matrix; // used to render geometry attached to this bone
    float world_matrix[16]; // used to attach
} fw64JointMatrices;

typedef struct {
    fw64AnimationData* animation_data;
    fw64Matrix* final_matrices;
    float* world_matrices;
    fw64Animation* current_animation;
    float current_time;

    /** playback speed modifier.  set to a negative value to play the animation in reverse */
    float speed;

    /** if set to a non zero value, the animation will play continuously */
    int loop;

    /** the current state of the animation player. */
    fw64AnimationState state;
} fw64AnimationController;

#ifdef __cplusplus
extern "C" {
#endif

/** initializes the animation controller
 * \param initial_animation the index of the initial animation to load from the underlying animation data.  If this value is negative, no initial animation will be loaded
 * */
void fw64_animation_controller_init(fw64AnimationController *controller, fw64AnimationData *animation_data, int initial_animation, fw64Allocator *allocator);

/** frees all resourced used by this animation controller.
 *  note: this will not free the underlying animation data.
 * */
void fw64_animation_controller_uninit(fw64AnimationController *controller, fw64Allocator *allocator);

/** sets the currently active animation from the underlying animation data. */
void fw64_animation_controller_set_animation(fw64AnimationController *controller, int index);

/** sets the current animation time.  The passed in value will be clamped between 0 and the total length of the current animation. */
void fw64_animation_controller_set_time(fw64AnimationController *controller, float time);

/** ticks the current animation time by time_delta */
void fw64_animation_controller_update(fw64AnimationController *controller, float time_delta);

/** begins animation playback if the animation is stopped or paused. */
void fw64_animation_controller_play(fw64AnimationController* controller);

/** pauses animation playback.  Does not adjust animation time at all */
void fw64_animation_controller_pause(fw64AnimationController* controller);

/** stops animation playback and resets the current animation time to 0 */
void fw64_animation_controller_stop(fw64AnimationController* controller);

float* fw64_animation_controller_get_world_matrix(fw64AnimationController* controller, uint32_t joint_index);

#ifdef __cplusplus
}
#endif
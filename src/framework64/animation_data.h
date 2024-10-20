#pragma once

/** \file animation_data.h */

#include "framework64/allocator.h"
#include "framework64/data_io.h"
#include "framework64/types.h"

#include <stdint.h>

typedef struct {
    uint16_t children_index;
    uint16_t children_count;
} fw64AnimationJoint;

typedef struct {
    uint32_t joint_count;
    fw64AnimationJoint* joints;
    uint16_t* hierarchy;
    float* inverse_bind_matrices;
} fw64AnimationSkin;

/** This stuct contains indices into the animation data buffer a single joint's channels.
 *  E.G. The Right leg bone for the run animation
 */
typedef struct {
    uint16_t translation_input_index;
    uint16_t translation;
    uint16_t rotation_input_index;
    uint16_t rotation;
    uint16_t scale_input_index;
    uint16_t scale;
} fw64AnimationTrack;

typedef struct {
    float total_time; // time of animation in seconds
    uint32_t tracks_index;  // index into the track array for the first joint's track
} fw64Animation;

typedef struct {
    fw64AnimationSkin skin;
    uint32_t animation_count;
    fw64Animation* animations;
    fw64AnimationTrack* tracks;
    float* keyframe_data;
} fw64AnimationData;

#ifdef __cplusplus
extern "C" {
#endif

fw64AnimationData* fw64_animation_data_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator);
void fw64_animation_data_delete(fw64AnimationData* animation_data, fw64Allocator* allocator);
fw64Animation* fw64_animation_data_get_animation(fw64AnimationData* animation_data, int index);

#ifdef __cplusplus
}
#endif

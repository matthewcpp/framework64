#pragma once

/** \file animation_data.h */

#include "framework64/allocator.h"
#include "framework64/asset_database.h"
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

/** indices into the animation data buffer */
typedef struct {
    uint16_t translation;
    uint16_t rotation;
    uint16_t scale;
} fw64AnimationTrack;

typedef struct {
    float total_time; // time of animation in seconds
    uint32_t key_count; // number of keys in the buffers
    uint32_t input_index; // offset into keyframe_data of input buffer
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

fw64AnimationData* fw64_animation_data_load(fw64AssetDatabase* database, uint32_t index, fw64Allocator* allocator);
void fw64_animation_data_delete(fw64AnimationData* animation_data, fw64Allocator* allocator);
fw64Animation* fw64_animation_data_get_animation(fw64AnimationData* animation_data, int index);


#ifdef __cplusplus
}
#endif


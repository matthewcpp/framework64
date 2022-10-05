#include "framework64/animation_data.h"
#include "framework64/filesystem.h"

typedef struct {
    uint32_t joint_count;
    uint32_t joint_hierarchy_size;
    uint32_t animation_count;
    uint32_t track_data_size;
    uint32_t keyframe_data_size;
} fw64AnimationInfo;

fw64AnimationData* fw64_animation_data_load(fw64AssetDatabase* database, uint32_t index, fw64Allocator* allocator) {
    if (allocator == NULL) allocator = fw64_default_allocator();

    int handle = fw64_filesystem_open(index);

    if (handle < 0)
        return NULL;

    fw64AnimationData* data = allocator->malloc(allocator, sizeof(fw64AnimationData));

    fw64AnimationInfo info;
    fw64_filesystem_read(&info, sizeof(fw64AnimationInfo), 1, handle);

    data->skin.joint_count = info.joint_count;
    char* data_ptr = allocator->malloc(allocator, info.joint_hierarchy_size);
    fw64_filesystem_read(data_ptr, info.joint_hierarchy_size, 1, handle);
    data->skin.joints = (fw64AnimationJoint *)data_ptr;
    data->skin.hierarchy = (uint16_t*)(data_ptr + info.joint_count * sizeof(fw64AnimationJoint));

    size_t inv_bind_matrices_size = info.joint_count * 16 * sizeof(float);
    data->skin.inverse_bind_matrices = allocator->malloc(allocator, inv_bind_matrices_size);
    fw64_filesystem_read(data->skin.inverse_bind_matrices, info.joint_count, 16 * sizeof(float), handle);

    data->animation_count = info.animation_count;
    data->animations = allocator->malloc(allocator, sizeof(fw64Animation) * info.animation_count);
    fw64_filesystem_read(data->animations, sizeof(fw64Animation), info.animation_count, handle);

    data->tracks = allocator->malloc(allocator, info.track_data_size);
    fw64_filesystem_read(data->tracks, 1, info.track_data_size, handle);

    data->keyframe_data = allocator->malloc(allocator, info.keyframe_data_size);
    fw64_filesystem_read(data->keyframe_data, 1, info.keyframe_data_size, handle);

    fw64_filesystem_close(handle);

    return data;
}

fw64Animation* fw64_animation_data_get_animation(fw64AnimationData* animation_data, int index) {
    return animation_data->animations + index;
}

void fw64_animation_data_delete(fw64AnimationData* data, fw64Allocator* allocator) {
    allocator->free(allocator, data->skin.joints); // note: joint hierarchy consists of single allocation
    allocator->free(allocator, data->skin.inverse_bind_matrices);
    allocator->free(allocator, data->animations);
    allocator->free(allocator, data->tracks);
    allocator->free(allocator, data->keyframe_data);
    allocator->free(allocator, data);
}

float* fw64_animation_data_get_inv_bind_matrix(fw64AnimationData* animation_data, int index) {
    return animation_data->skin.inverse_bind_matrices + (index * 16);
}

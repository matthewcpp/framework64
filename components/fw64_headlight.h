#pragma once

#include "framework64/transform.h"
#include "framework64/render_pass.h"
#include "framework64/util/sparse_set.h"

typedef struct {
    fw64RenderPass* renderpass;
    int light_index;
    fw64Transform* transform;
} fw64Headlight;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_headlight_init(fw64Headlight* headlight, fw64RenderPass* renderpass, int light_index, fw64Transform* transform);
void fw64_headlight_update(fw64Headlight* headlight);

typedef struct {
    fw64SparseSet components;
} fw64Headlights;

typedef fw64SparseSetHandle fw64HeadlightHandle;

void fw64_headlights_init(fw64Headlights* headlights, fw64Allocator* allocator);
void fw64_headlights_update(fw64Headlights* headlights);
fw64Headlight* fw64_headlights_create(fw64Headlights* headlights, fw64RenderPass* renderpass, int light_index, fw64Transform* transform, fw64HeadlightHandle* out_handle);
void fw64_headlights_delete(fw64Headlights* headlights, fw64SparseSetHandle handle);

#ifdef __cplusplus
}
#endif

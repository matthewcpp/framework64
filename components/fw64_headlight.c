#include "fw64_headlight.h"

void fw64_headlight_init(fw64Headlight* headlight, fw64RenderPass* renderpass, int light_index, fw64Transform* transform) {
    headlight->renderpass = renderpass;
    headlight->light_index = light_index;
    headlight->transform = transform;
}

void fw64_headlight_update(fw64Headlight* headlight) {
    Vec3 forward;
    fw64_transform_forward(headlight->transform, &forward);
    fw64_renderpass_set_light_direction(headlight->renderpass, headlight->light_index, &forward);
}

void fw64_headlights_init(fw64Headlights* headlights, fw64Allocator* allocator) {
    fw64_sparse_set_init(&headlights->components, sizeof(fw64Headlight), allocator);
}

void fw64_headlights_update(fw64Headlights* headlights) {
    fw64SparseSetItr itr;
    for (fw64_sparse_set_itr_init(&itr, &headlights->components); fw64_sparse_set_itr_has_next(&itr); fw64_sparse_set_itr_next(&itr)) {
        fw64Headlight* headlight = fw64_sparse_set_itr_get_item(&itr);
        fw64_headlight_update(headlight);
    }
}

fw64Headlight* fw64_headlights_create(fw64Headlights* headlights, fw64RenderPass* renderpass, int light_index, fw64Transform* transform, fw64HeadlightHandle* out_handle) {
    fw64HeadlightHandle handle;
    fw64Headlight* headlight = fw64_sparse_set_alloc(&headlights->components, &handle);
    fw64_headlight_init(headlight, renderpass, light_index, transform);

    if (out_handle) {
        *out_handle = handle;
    }

    return headlight;
}

void fw64_headlights_delete(fw64Headlights* headlights, fw64SparseSetHandle handle) {
    fw64_sparse_set_delete(&headlights->components, handle);
}

#include "fw64_headlight.h"

void fw64_headlight_init(fw64Headlight* headlight, fw64RenderPass* renderpass, int light_index, fw64Camera* camera) {
    headlight->renderpass = renderpass;
    headlight->light_index = light_index;
    headlight->camera = camera;
}

void fw64_headlight_update(fw64Headlight* headlight) {
    Vec3 forward;
    fw64_transform_forward(&headlight->camera->transform, &forward);
    fw64_renderpass_set_light_direction(headlight->renderpass, headlight->light_index, &forward);
}

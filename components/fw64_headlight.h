#pragma once

#include "framework64/camera.h"
#include "framework64/render_pass.h"


typedef struct {
    fw64RenderPass* renderpass;
    int light_index;
    fw64Camera* camera;
} fw64Headlight;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_headlight_init(fw64Headlight* headlight, fw64RenderPass* renderpass, int light_index, fw64Camera* camera);
void fw64_headlight_update(fw64Headlight* headlight);

#ifdef __cplusplus
}
#endif
#pragma once

#include "framework64/camera.h"
#include "framework64/engine.h"
#include "framework64/transform.h"

typedef struct {
    fw64Engine* engine;
    fw64Camera camera;
    fw64Transform* target;

    float target_follow_dist;
    float target_follow_height;
    float target_forward_dist;
    float target_forward_height;
    float camera_adjust_speed;
} ChaseCamera;

#ifdef __cplusplus
extern "C" {
#endif

void chase_camera_init(ChaseCamera* chase_cam, fw64Engine* engine);
void chase_camera_update(ChaseCamera* chase_cam);

#ifdef __cplusplus
}
#endif

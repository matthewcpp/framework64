#pragma once

#include "framework64/camera.h"
#include "framework64/transform.h"

typedef struct {
    fw64Camera camera;
    fw64Transform* target;
} ChaseCamera;

#ifdef __cplusplus
extern "C" {
#endif

void chase_camera_init(ChaseCamera* chase_cam);
void chase_camera_update(ChaseCamera* chase_cam);

#ifdef __cplusplus
}
#endif
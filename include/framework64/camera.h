#pragma once

/** \file camera.h */

#include "framework64/frustum.h"
#include "framework64/transform.h"
#include "framework64/types.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    fw64Transform transform;
    fw64Matrix projection;
    fw64Matrix view;
    float near, far;
    float fovy;
    float aspect;
    uint16_t perspNorm;
} fw64Camera;

void fw64_camera_init(fw64Camera* camera);
void fw64_camera_update_projection_matrix(fw64Camera* camera);
void fw64_camera_update_view_matrix(fw64Camera* camera);

void fw64_camera_extract_frustum_planes(fw64Camera* camera, fw64Frustum* planes);

#ifdef __cplusplus
}
#endif

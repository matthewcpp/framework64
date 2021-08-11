#ifndef FW64_CAMERA_H
#define FW64_CAMERA_H

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

#ifdef __cplusplus
}
#endif

#endif
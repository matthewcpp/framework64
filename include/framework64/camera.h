#ifndef FW64_CAMERA_H
#define FW64_CAMERA_H

#include "transform.h"

#include <nusys.h>


typedef struct {
    fw64Transform transform;
    Mtx projection;
    Mtx view;
    float near, far;
    float fovy;
    float aspect;
    u16 perspNorm;
} fw64Camera;

void fw64_camera_init(fw64Camera* camera);
void fw64_camera_update_projection_matrix(fw64Camera* camera);
void fw64_camera_update_view_matrix(fw64Camera* camera);

#endif
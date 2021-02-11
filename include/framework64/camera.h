#ifndef FW64_CAMERA_H
#define FW64_CAMERA_H

#include "transform.h"

#include <nusys.h>


typedef struct {
    Transform transform;
    Mtx projection;
    Mtx view;
    float near, far;
    float fovy;
    float aspect;
    u16 perspNorm;
} Camera;

void camera_init(Camera* camera);
void camera_update_projection_matrix(Camera* camera);
void camera_update_view_matrix(Camera* camera);

#endif
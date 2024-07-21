#pragma once

/** \file arcball_camera.h */

#include "framework64/camera.h"
#include "framework64/display.h"
#include "framework64/input.h"
#include "framework64/box.h"

typedef struct {
    fw64Input* _input;
    fw64Camera camera;

    float _distance;
    float _diagonal;
    float _rot_x;
    float _rot_y;
    Vec3 _target;
} fw64ArcballCamera;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_arcball_init(fw64ArcballCamera* arcball, fw64Input* input, fw64Display* display);
void fw64_arcball_set_initial(fw64ArcballCamera* arcball, Box* box);
void fw64_arcball_update(fw64ArcballCamera* arcball, float time_delta);

#ifdef __cplusplus
}
#endif

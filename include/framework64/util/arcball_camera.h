#ifndef ARCBALL_H
#define ARCBALL_H

#include "framework64/camera.h"
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
} ArcballCamera;

#ifdef __cplusplus
extern "C" {
#endif

void arcball_init(ArcballCamera* arcball, fw64Input* input);
void arcball_set_initial(ArcballCamera* arcball, Box* box);
void arcball_update(ArcballCamera* arcball, float time_delta);

#ifdef __cplusplus
}
#endif

#endif
#ifndef ARCBALL_H
#define ARCBALL_H

#include "framework64/camera.h"
#include "framework64/input.h"
#include "framework64/box.h"

typedef struct {
    Input* _input;
    Camera camera;

    float _distance;
    float _diagonal;
    float _rot_x;
    float _rot_y;
    Vec3 _target;
} ArcballCamera;

void arcball_init(ArcballCamera* arcball, Input* input);
void arcball_set_initial(ArcballCamera* arcball, Box* box);
void arcball_update(ArcballCamera* arcball, float time_delta);

#endif
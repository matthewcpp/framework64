#ifndef ARCBALL_H
#define ARCBALL_H

#include "ultra/camera.h"
#include "ultra/input.h"
#include "ultra/box.h"

typedef struct {
    Camera* _camera;
    Input* _input;

    float _distance;
    float _diagonal;
    float _rot_x;
    float _rot_y;
    Vec3 _target;
} ArcballCamera;

ArcballCamera* arcball_create(Camera* camera, Input* input);
void arcball_set_initial(ArcballCamera* arcball, Box* box);
void arcball_update(ArcballCamera* arcball, float time_delta);

#endif
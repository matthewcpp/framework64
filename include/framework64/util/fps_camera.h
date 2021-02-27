#ifndef GOLDENEYE_CAMERA_H
#define GOLDENEYE_CAMERA_H

#include "framework64/camera.h"
#include "framework64/input.h"
#include "framework64/vec2.h"

typedef struct {
    Camera camera;
    Input* input;
    Vec2 rotation;
} FpsCamera;

void fps_camera_init(FpsCamera* fps, Input* input);
void fps_camera_update(FpsCamera* fps, float time_delta);

#endif
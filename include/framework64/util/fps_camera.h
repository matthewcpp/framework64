#pragma once

/** \file fps_camera.h */

#include "framework64/camera.h"
#include "framework64/input.h"
#include "framework64/vec2.h"

typedef struct {
    fw64Camera camera;
    float movement_speed;
    float turn_speed;
    int player_index;
    fw64Input* _input;
    Vec2 _rotation;
} fw64FpsCamera;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_fps_camera_init(fw64FpsCamera *fps, fw64Input *input);
void fw64_fps_camera_update(fw64FpsCamera *fps, float time_delta);

#ifdef __cplusplus
}
#endif

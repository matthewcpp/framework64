#pragma once

#include "framework64/engine.h"

typedef struct {
    fw64Camera* camera;
    fw64Transform* target;
    fw64Engine* engine;

    float distance;
} fw64ThirdPersonCamera;

void fw64_third_person_camera_init(fw64ThirdPersonCamera* cam, fw64Engine* engine, fw64Camera* camera, fw64Transform* target);
void fw64_third_person_camera_update(fw64ThirdPersonCamera* cam);

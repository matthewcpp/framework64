#pragma once

#include <framework64/camera.h>

typedef struct {
    fw64Transform* target;
    fw64Camera* camera;
    Vec3 offset_vec;
} fw64ThirdPersonCamera;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_third_person_camera_init(fw64ThirdPersonCamera* cam, fw64Transform* target, fw64Camera* camera);
void fw64_third_person_camera_update(fw64ThirdPersonCamera* cam);

#ifdef __cplusplus
}
#endif

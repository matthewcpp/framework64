#pragma once

#include <framework64/camera.h>

#define FW64_THIRD_PERSON_CAMERA_DEFAULT_FOLLOW_DISTANCE 20.0f

typedef struct {
    fw64Transform* target;
    fw64Camera* camera;
    float follow_dist;
    float offset_height; // Temp replace with rotation z angle in future

    /** degrees */
    float rotation_y;

    /** degrees */
    float rotation_z;
} fw64ThirdPersonCamera;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_third_person_camera_init(fw64ThirdPersonCamera* cam, fw64Transform* target, fw64Camera* camera);
void fw64_third_person_camera_update(fw64ThirdPersonCamera* cam);

/** Sets the camera to be behind the target at the default offset values */
void fw64_third_person_camera_reset(fw64ThirdPersonCamera* cam);

#ifdef __cplusplus
}
#endif

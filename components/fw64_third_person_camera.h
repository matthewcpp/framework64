#pragma once

#include <framework64/camera.h>

#define FW64_THIRD_PERSON_CAMERA_DEFAULT_FOLLOW_DISTANCE 5.0f

typedef struct {
    fw64Transform* target;
    fw64Camera* camera;

    /** The distance the camera will be from the target */
    float follow_dist;

    /** An offset added to the target's position.
     *  The sum of these two vector's will be used for the camera's look at target */
    Vec3 target_offset;

    /** Expressed in degrees */
    float rotation_y;

    /** Expressed in degrees */
    float rotation_x;
} fw64ThirdPersonCamera;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_third_person_camera_init(fw64ThirdPersonCamera* cam, fw64Transform* target, fw64Camera* camera);
void fw64_third_person_camera_update(fw64ThirdPersonCamera* cam);

/** Sets the camera to be behind the target at the default offset values */
void fw64_third_person_camera_reset(fw64ThirdPersonCamera* cam);

/** Rotates the camera about each axis. It is preferred to use this method as opposed to setting values directly.
 *  Note: these values are deltas from the current value and are expressed in degrees.
 */
void fw64_third_person_camera_rotate(fw64ThirdPersonCamera* cam, float x, float y);

#ifdef __cplusplus
}
#endif

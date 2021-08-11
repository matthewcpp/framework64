#include "framework64/camera.h"

#include "gtest/gtest.h"

TEST(Camera, ViewMatrix) {
    fw64Camera camera;
    fw64_camera_init(&camera);

    vec3_set(&camera.transform.position, 12.67449f, 8.872139f, 12.67449f);
    quat_set(&camera.transform.rotation, -0.08717229f, 0.8995905f, -0.2104525f, -0.3726227f);

    camera.aspect = 1.3333f;
    camera.fovy = 60.0f;
    camera.near = 0.3f;
    camera.far = 1000.0f;

    fw64_camera_update_view_matrix(&camera);
}

TEST(Camera, PerspectiveMatrix) {
    fw64Camera camera;
    fw64_camera_init(&camera);

    camera.aspect = 1.3333f;
    camera.fovy = 60.0f;
    camera.near = 0.3f;
    camera.far = 1000.0f;

    fw64_camera_update_projection_matrix(&camera);
}
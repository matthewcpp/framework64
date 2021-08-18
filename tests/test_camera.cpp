#include "framework64/camera.h"

#include "gtest/gtest.h"

TEST(Camera, PerspectiveMatrix) {
    fw64Camera camera;

    camera.aspect = 1.3333f;
    camera.fovy = 60.0f;
    camera.near = 0.3f;
    camera.far = 1000.0f;

    fw64_camera_update_projection_matrix(&camera);

    float expectedProjection[16] = {1.29907048, 0, 0, 0, 0, 1.73205066, 0, 0, 0, 0, -1.0006001, -1, 0,0, -0.60018003, 0};

    for (int i = 0; i < 16; i++) {
        ASSERT_NEAR(camera.projection.m[i], expectedProjection[i], 0.001);
    }
}

TEST(Camera, ViewMatrix) {
    fw64Camera camera;

    vec3_set(&camera.transform.position, 0.0f, 0.0f, 10.0f);
    Vec3 target = {0.0f, 0.0f, 0.0f};
    Vec3 up = {0.0f, 1.0f, 0.0f};
    fw64_transform_up(&camera.transform, &up);
    fw64_transform_look_at(&camera.transform, &target, &up);

    fw64_camera_update_view_matrix(&camera);

    float expectedView[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0 ,0, 1, 0, 0, 0, -10, 1};

    for (int i = 0; i < 16; i++) {
        EXPECT_NEAR(camera.view.m[i], expectedView[i], 0.001);
    }
}

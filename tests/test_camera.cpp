#include "framework64/camera.h"
#include "framework64/desktop/display.hpp"
#include "gtest/gtest.h"

TEST(Camera, PerspectiveMatrix) {
    fw64Display display;
    display.initWithoutWindow(32, 240);

    fw64Node camera_node;
    fw64_node_init(&camera_node);

    fw64Camera camera;
    fw64_camera_init(&camera, &camera_node, &display);

    camera.aspect = 1.3333f;
    camera.fovy = 60.0f;
    camera.near = 0.3f;
    camera.far = 1000.0f;

    fw64_camera_update_projection_matrix(&camera);

    float expectedProjection[16] = {1.29907048f, 0.0f, 0.0f, 0.0f, 0.0f, 1.73205066f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0006001f, -1.0f, 0.0f, 0.0f, -0.60018003f, 0.0f};

    for (int i = 0; i < 16; i++) {
        ASSERT_NEAR(camera.projection.m[i], expectedProjection[i], 0.001);
    }
}

TEST(Camera, ViewMatrix) {
    fw64Display display;
    display.initWithoutWindow(32, 240);

    fw64Node camera_node;
    fw64_node_init(&camera_node);
    vec3_set(&camera_node.transform.position, 0.0f, 0.0f, 10.0f);
    fw64_node_update(&camera_node);

    fw64Camera camera;
    fw64_camera_init(&camera, &camera_node, &display);

    Vec3 target = {0.0f, 0.0f, 0.0f};
    Vec3 up = vec3_up();
    fw64_transform_up(&camera_node.transform, &up);
    fw64_transform_look_at(&camera_node.transform, &target, &up);

    fw64_camera_update_view_matrix(&camera);

    float expectedView[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0 ,0, 1, 0, 0, 0, -10, 1};

    for (int i = 0; i < 16; i++) {
        EXPECT_NEAR(camera.view.m[i], expectedView[i], 0.001);
    }
}

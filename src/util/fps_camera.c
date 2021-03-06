#include "framework64/util/fps_camera.h"

void fps_camera_init(FpsCamera* fps, Input* input){
    fps->input = input;

    camera_init(&fps->camera);
    vec2_set(&fps->rotation, 0.0f, 0.0f);
}

#define MOVEMENT_SPEED 8.0f
#define ROTATION_SPEED 180.0f
#define STICK_THRESHOLD 0.15

static void fps_cam_forward(FpsCamera* fps, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, fps->rotation.y, 0.0f);

    Vec3 forward = { 0.0f, 0.0f, -1.0f };
    quat_transform_vec3(out, &q, &forward);
}

static void fps_cam_back(FpsCamera* fps, Vec3* out) {
    fps_cam_forward(fps, out);
    vec3_negate(out);
}

static void fps_cam_right(FpsCamera* fps, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, fps->rotation.y, 0.0f);

    Vec3 right = { 1.0f, 0.0f, 0.0f };
    quat_transform_vec3(out, &q, &right);
}

static void fps_cam_left(FpsCamera* fps, Vec3* out) {
    fps_cam_right(fps, out);
    vec3_negate(out);
}

static void move_camera(FpsCamera* fps, float time_delta, Vec2* stick) {
    if (input_button_down(fps->input, 0, R_CBUTTONS)) {
        Vec3 move;
        fps_cam_right(fps, &move);
        vec3_scale(&move, &move, MOVEMENT_SPEED * time_delta);
        vec3_add(&fps->camera.transform.position, &fps->camera.transform.position, &move);
    }

    if (input_button_down(fps->input, 0, L_CBUTTONS)) {
        Vec3 move;
        fps_cam_left(fps, &move);
        vec3_scale(&move, &move, MOVEMENT_SPEED * time_delta);
        vec3_add(&fps->camera.transform.position, &fps->camera.transform.position, &move);
    }

    if (stick->y > STICK_THRESHOLD) {
        Vec3 move;
        fps_cam_forward(fps, &move);
        vec3_scale(&move, &move, MOVEMENT_SPEED * time_delta * stick->y);
        vec3_add(&fps->camera.transform.position, &fps->camera.transform.position, &move);
    }

        if (stick->y < -STICK_THRESHOLD) {
        Vec3 move;
        fps_cam_back(fps, &move);
        vec3_scale(&move, &move, MOVEMENT_SPEED * time_delta * -stick->y);
        vec3_add(&fps->camera.transform.position, &fps->camera.transform.position, &move);
    }
}

static void tilt_camera(FpsCamera* fps, float time_delta, Vec2* stick) {
    if (stick->x > STICK_THRESHOLD) {
        fps->rotation.y -= ROTATION_SPEED * time_delta;
    }

    if (stick->x < -STICK_THRESHOLD) {
        fps->rotation.y += ROTATION_SPEED * time_delta;
    }

    if (input_button_down(fps->input, 0, U_CBUTTONS)) {
        fps->rotation.x += ROTATION_SPEED * time_delta;

        if (fps->rotation.x > 90.0f)
            fps->rotation.x = 90.0f;
    }

    if (input_button_down(fps->input, 0, D_CBUTTONS)) {
        fps->rotation.x -= ROTATION_SPEED * time_delta;

        if (fps->rotation.x < -90.0f)
            fps->rotation.x = -90.0f;
    }
}

void fps_camera_update(FpsCamera* fps, float time_delta) {
    Vec2 stick;
    input_stick(fps->input, 0, &stick);

    move_camera(fps, time_delta, &stick);
    tilt_camera(fps, time_delta, &stick);

    Quat q;
    quat_from_euler(&q, fps->rotation.x, fps->rotation.y, 0.0f);

    Vec3 forward = { 0.0f, 0.0f, -1.0f };
    Vec3 tar;
    quat_transform_vec3(&tar, &q, &forward);
    vec3_add(&tar, &fps->camera.transform.position, &tar);

    Vec3 up = {0.0f, 1.0f, 0.0f};

    transform_look_at(&fps->camera.transform, &tar, &up);
    camera_update_view_matrix(&fps->camera);
}
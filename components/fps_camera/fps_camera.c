#include "fps_camera/fps_camera.h"

#include "framework64/n64/controller_button.h"

#define DEFAULT_MOVEMENT_SPEED 8.0f
#define DEFAULT_TURN_SPEED 90.0f
#define STICK_THRESHOLD 0.15

void fw64_fps_camera_init(fw64FpsCamera* fps, fw64Input* input){
    fps->_input = input;

    fps->movement_speed = DEFAULT_MOVEMENT_SPEED;
    fps->turn_speed = DEFAULT_TURN_SPEED;

    fps->player_index = 0;

    fw64_camera_init(&fps->camera);
    vec2_set(&fps->rotation, 0.0f, 0.0f);
}

static void fps_cam_forward(fw64FpsCamera* fps, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, fps->rotation.y, 0.0f);

    Vec3 forward = { 0.0f, 0.0f, -1.0f };
    quat_transform_vec3(out, &q, &forward);
}

static void fps_cam_back(fw64FpsCamera* fps, Vec3* out) {
    fps_cam_forward(fps, out);
    vec3_negate(out);
}

static void fps_cam_right(fw64FpsCamera* fps, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, fps->rotation.y, 0.0f);

    Vec3 right = { 1.0f, 0.0f, 0.0f };
    quat_transform_vec3(out, &q, &right);
}

static void fps_cam_left(fw64FpsCamera* fps, Vec3* out) {
    fps_cam_right(fps, out);
    vec3_negate(out);
}

static void move_camera(fw64FpsCamera* fps, float time_delta, Vec2* stick) {
    if (fw64_input_controller_button_down(fps->_input, fps->player_index, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        Vec3 move;
        fps_cam_right(fps, &move);
        vec3_scale(&move, &move, fps->movement_speed * time_delta);
        vec3_add(&fps->camera.transform.position, &fps->camera.transform.position, &move);
    }

    if (fw64_input_controller_button_down(fps->_input, fps->player_index, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        Vec3 move;
        fps_cam_left(fps, &move);
        vec3_scale(&move, &move, fps->movement_speed * time_delta);
        vec3_add(&fps->camera.transform.position, &fps->camera.transform.position, &move);
    }

    if (stick->y > STICK_THRESHOLD) {
        Vec3 move;
        fps_cam_forward(fps, &move);
        vec3_scale(&move, &move, fps->movement_speed * time_delta * stick->y);
        vec3_add(&fps->camera.transform.position, &fps->camera.transform.position, &move);
    }

    if (stick->y < -STICK_THRESHOLD) {
        Vec3 move;
        fps_cam_back(fps, &move);
        vec3_scale(&move, &move, fps->movement_speed * time_delta * -stick->y);
        vec3_add(&fps->camera.transform.position, &fps->camera.transform.position, &move);
    }
}

static void tilt_camera(fw64FpsCamera* fps, float time_delta, Vec2* stick) {
    if (stick->x > STICK_THRESHOLD) {
        fps->rotation.y -= fps->turn_speed * time_delta;
    }

    if (stick->x < -STICK_THRESHOLD) {
        fps->rotation.y += fps->turn_speed * time_delta;
    }

    if (fw64_input_controller_button_down(fps->_input, fps->player_index, FW64_N64_CONTROLLER_BUTTON_C_UP)) {
        fps->rotation.x += fps->turn_speed * time_delta;

        if (fps->rotation.x > 90.0f)
            fps->rotation.x = 90.0f;
    }

    if (fw64_input_controller_button_down(fps->_input, fps->player_index, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        fps->rotation.x -= fps->turn_speed * time_delta;

        if (fps->rotation.x < -90.0f)
            fps->rotation.x = -90.0f;
    }
}

void fw64_fps_camera_update(fw64FpsCamera* fps, float time_delta) {
    Vec2 stick;
    fw64_input_controller_stick(fps->_input, fps->player_index, &stick);

    move_camera(fps, time_delta, &stick);
    tilt_camera(fps, time_delta, &stick);

    Quat q;
    quat_from_euler(&q, fps->rotation.x, fps->rotation.y, 0.0f);

    Vec3 forward = { 0.0f, 0.0f, -1.0f };
    Vec3 tar;
    quat_transform_vec3(&tar, &q, &forward);
    vec3_add(&tar, &fps->camera.transform.position, &tar);

    Vec3 up = {0.0f, 1.0f, 0.0f};

    fw64_transform_look_at(&fps->camera.transform, &tar, &up);
    fw64_camera_update_view_matrix(&fps->camera);
}
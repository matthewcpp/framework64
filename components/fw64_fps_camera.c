#include "fw64_fps_camera.h"

#include "framework64/controller_mapping/n64.h"

#define DEFAULT_MOVEMENT_SPEED 8.0f
#define DEFAULT_TURN_SPEED 90.0f
#define STICK_THRESHOLD 0.15

#define MAX_ROTATION_X 89.0f
#define MIN_ROTATION_X -89.0f

void fw64_fps_camera_init(fw64FpsCamera* fps, fw64Input* input, fw64Camera* camera){
    fps->camera = camera;
    fps->_input = input;

    fps->movement_speed = DEFAULT_MOVEMENT_SPEED;
    fps->turn_speed = DEFAULT_TURN_SPEED;

    fps->player_index = 0;

    vec2_set(&fps->rotation, 0.0f, 0.0f);
}

static void fps_cam_forward(fw64FpsCamera* fps, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, fps->rotation.y, 0.0f);

    Vec3 forward = vec3_forward();
    quat_transform_vec3(&q, &forward, out);
}

static void fps_cam_back(fw64FpsCamera* fps, Vec3* out) {
    fps_cam_forward(fps, out);
    vec3_negate(out);
}

static void fps_cam_right(fw64FpsCamera* fps, Vec3* out) {
    Quat q;
    quat_from_euler(&q, 0.0f, fps->rotation.y, 0.0f);

    Vec3 right = vec3_right();
    quat_transform_vec3(&q, &right, out);
}

static void fps_cam_left(fw64FpsCamera* fps, Vec3* out) {
    fps_cam_right(fps, out);
    vec3_negate(out);
}

static void move_camera(fw64FpsCamera* fps, float time_delta, Vec2* stick) {
    fw64Transform* transform = &fps->camera->node->transform;
    if (fw64_input_controller_button_down(fps->_input, fps->player_index, FW64_N64_CONTROLLER_BUTTON_C_RIGHT)) {
        Vec3 move;
        fps_cam_right(fps, &move);
        vec3_scale(&move, fps->movement_speed * time_delta, &move);
        vec3_add(&transform->position, &move, &transform->position);
    }

    if (fw64_input_controller_button_down(fps->_input, fps->player_index, FW64_N64_CONTROLLER_BUTTON_C_LEFT)) {
        Vec3 move;
        fps_cam_left(fps, &move);
        vec3_scale(&move, fps->movement_speed * time_delta, &move);
        vec3_add(&transform->position, &move, &transform->position);
    }

    if (stick->y > STICK_THRESHOLD) {
        Vec3 move;
        fps_cam_forward(fps, &move);
        vec3_scale(&move, fps->movement_speed * time_delta * stick->y, &move);
        vec3_add(&transform->position, &move, &transform->position);
    }

    if (stick->y < -STICK_THRESHOLD) {
        Vec3 move;
        fps_cam_back(fps, &move);
        vec3_scale(&move, fps->movement_speed * time_delta * -stick->y, &move);
        vec3_add(&transform->position, &move, &transform->position);
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

        if (fps->rotation.x > MAX_ROTATION_X)
            fps->rotation.x = MAX_ROTATION_X;
    }

    if (fw64_input_controller_button_down(fps->_input, fps->player_index, FW64_N64_CONTROLLER_BUTTON_C_DOWN)) {
        fps->rotation.x -= fps->turn_speed * time_delta;

        if (fps->rotation.x < MIN_ROTATION_X)
            fps->rotation.x = MIN_ROTATION_X;
    }
}

void fw64_fps_camera_update(fw64FpsCamera* fps, float time_delta) {
    fw64Transform* transform = &fps->camera->node->transform;

    Vec2 stick;
    fw64_input_controller_stick(fps->_input, fps->player_index, &stick);

    move_camera(fps, time_delta, &stick);
    tilt_camera(fps, time_delta, &stick);

    Quat q;
    quat_from_euler(&q, fps->rotation.x, fps->rotation.y, 0.0f);

    Vec3 forward = vec3_forward();
    Vec3 tar;
    quat_transform_vec3(&q, &forward, &tar);
    vec3_add(&transform->position, &tar, &tar);

    Vec3 up = vec3_up();

    fw64_transform_look_at(transform, &tar, &up);
    fw64_camera_update_view_matrix(fps->camera);
    fw64_node_update(fps->camera->node);
}

#include "fw64_arcball_camera.h"

#include "framework64/controller_mapping/n64.h"

#define ARCBALL_ORBIT_SPEED 180.0f
#define ARCBALL_ZOOM_SPEED 1.0f
#define ARCBALL_DEAD_ZONE 0.2f

void fw64_arcball_init(fw64ArcballCamera* arcball, fw64Input* input, fw64Camera* camera) {
    arcball->camera = camera;
    arcball->_input = input;
    arcball->_diagonal = 1.0f;

    fw64_arcball_reset(arcball);
    vec3_zero(&arcball->_target);
}

void fw64_arcball_reset(fw64ArcballCamera* arcball) {
    arcball->_rot_x = 0.0f;
    arcball->_rot_y = 0.0f;
    arcball->_distance = arcball->_diagonal * 2.0f;
}

void _arcball_update_camera_position(fw64ArcballCamera* arcball) {
    if (arcball->_rot_x > 90.0f)
        arcball->_rot_x = 90.0f;
    if (arcball->_rot_x < -90.0f)
        arcball->_rot_x = -90.0f;

    Quat q;
    quat_from_euler(&q, arcball->_rot_x, arcball->_rot_y, 0.0f);

    Vec3 up = {0.0f, 1.0f, 0.0f};
    quat_transform_vec3(&up, &q, &up);
    vec3_normalize(&up);

    Vec3 orbit_pos = {0.0f, 0.0f, 1.0f};
    quat_transform_vec3(&orbit_pos, &q, &orbit_pos);
    vec3_normalize(&orbit_pos);
    vec3_scale(&orbit_pos, arcball->_distance, &orbit_pos);

    fw64Transform* transform = &arcball->camera->node->transform;
    vec3_add(&arcball->_target, &orbit_pos, &transform->position);

    fw64_transform_look_at(transform, &arcball->_target, &up);
    fw64_camera_update_view_matrix(arcball->camera);
    fw64_node_update(arcball->camera->node);
}

void fw64_arcball_set_initial(fw64ArcballCamera* arcball, Box* box) {
    box_center(box, &arcball->_target);
    arcball->_diagonal = vec3_distance(&box->min, &box->max);
    fw64_arcball_reset(arcball);

    _arcball_update_camera_position(arcball);
}

void fw64_arcball_update(fw64ArcballCamera* arcball, float time_delta) {
    Vec2 stick;
    fw64_input_controller_stick(arcball->_input, 0, &stick);

    if (stick.x > ARCBALL_DEAD_ZONE) {
        arcball->_rot_y -= ARCBALL_ORBIT_SPEED * time_delta;
    }
    else if (stick.x < -ARCBALL_DEAD_ZONE) {
        arcball->_rot_y += ARCBALL_ORBIT_SPEED * time_delta;
    }

    if (stick.y > ARCBALL_DEAD_ZONE) {
        arcball->_rot_x += ARCBALL_ORBIT_SPEED * time_delta;
    }
    else if (stick.y < -ARCBALL_DEAD_ZONE) {
        arcball->_rot_x -= ARCBALL_ORBIT_SPEED * time_delta;
    }

    if (fw64_input_controller_button_down(arcball->_input, 0, FW64_N64_CONTROLLER_BUTTON_L)) {
        arcball->_distance += arcball->_diagonal * ARCBALL_ZOOM_SPEED * time_delta;
    }
    else if (fw64_input_controller_button_down(arcball->_input, 0, FW64_N64_CONTROLLER_BUTTON_R)) {
        arcball->_distance -= arcball->_diagonal * ARCBALL_ZOOM_SPEED * time_delta;
    }

    _arcball_update_camera_position(arcball);
}

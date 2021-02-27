#include "framework64/util/arcball_camera.h"

#include <malloc.h>

#define ARCBALL_ORBIT_SPEED 180.0f
#define ARCBALL_ZOOM_SPEED 1.0f
#define ARCBALL_DEAD_ZONE 0.2f

static void _arcball_reset(ArcballCamera* arcball) {
    arcball->_rot_x = 0.0f;
    arcball->_rot_y = 0.0f;
    arcball->_distance = arcball->_diagonal * 2.0f;
}

void arcball_init(ArcballCamera* arcball, Input* input) {
    arcball->_input = input;

    camera_init(&arcball->camera);

    arcball->_diagonal = 1.0f;
    _arcball_reset(arcball);
    vec3_zero(&arcball->_target);
}

void _arcball_update_camera_position(ArcballCamera* arcball) {
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
    vec3_scale(&orbit_pos, &orbit_pos, arcball->_distance);

    vec3_add(&arcball->camera.transform.position, &arcball->_target, &orbit_pos);

    transform_look_at(&arcball->camera.transform, &arcball->_target, &up);
    camera_update_view_matrix(&arcball->camera);
}

void arcball_set_initial(ArcballCamera* arcball, Box* box) {
    box_center(box, &arcball->_target);
    arcball->_diagonal = vec3_distance(&box->min, &box->max);
    _arcball_reset(arcball);

    _arcball_update_camera_position(arcball);
}

void arcball_update(ArcballCamera* arcball, float time_delta) {
    Vec2 stick;
    input_stick(arcball->_input, 0, &stick);

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

    if (input_button_down(arcball->_input, 0, L_TRIG)) {
        arcball->_distance += arcball->_diagonal * ARCBALL_ZOOM_SPEED * time_delta;
    }
    else if (input_button_down(arcball->_input, 0, R_TRIG)) {
        arcball->_distance -= arcball->_diagonal * ARCBALL_ZOOM_SPEED * time_delta;
    } 

    if (input_button_pressed(arcball->_input, 0, START_BUTTON)) {
        _arcball_reset(arcball);
    }

    _arcball_update_camera_position(arcball);
}
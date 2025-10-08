#include "fw64_third_person_camera.h"

#include <framework64/log.h>
#include <framework64/math.h>
#include <framework64/matrix.h>

#include <stdio.h>
#include <math.h>

void fw64_third_person_camera_init(fw64ThirdPersonCamera* cam, fw64Camera* camera, fw64Character* character, fw64CharacterAnimationController* anim_controller){
    cam->camera = camera;
    cam->character = character;
    cam->anim_controller = anim_controller;

    cam->min_follow_distance = FW64_THIRD_PERSON_CAMERA_MIN_FOLLOW_DISTANCE;
    cam->max_follow_distance = FW64_THIRD_PERSON_CAMERA_MAX_FOLLOW_DISTANCE;

    fw64_third_person_camera_reset(cam);
}

void fw64_third_person_camera_set_follow_distance(fw64ThirdPersonCamera* cam, float follow_distance) {
    cam->follow_dist = fw64_clamp(follow_distance, cam->min_follow_distance, cam->max_follow_distance);
}

#define CAMERA_COLLISION_GEOMETRY_MASK (FW64_COLLISION_GEOMETRY_TYPE_FLOOR | FW64_COLLISION_GEOMETRY_TYPE_WALL | FW64_COLLISION_GEOMETRY_TYPE_CEILING)

/**
 * Computes the camera final position given a base and target position
 * This is accomplished by performing a raycast against the world geometry and performing a collison response if necessary
 */
static void fw64_third_person_camera_resolve_world_collisions(fw64ThirdPersonCamera* cam, const Vec3* target_pos, const Vec3* cam_pos) {
    Vec3 ray_dir, final_pos;
    vec3_subtract(cam_pos, target_pos, &ray_dir);
    vec3_normalize(&ray_dir);
    const float ray_dist = vec3_distance(target_pos, cam_pos);

    const fw64CollisionTriangle* triangle = fw64_collision_geometry_raycast_triangle(cam->character->scene->collision_geometry, target_pos, &ray_dir, 0.1f, ray_dist, CAMERA_COLLISION_GEOMETRY_MASK, &final_pos);
    if (triangle) {
        cam->camera->node->transform.position = final_pos;
    } else {
        cam->camera->node->transform.position = *cam_pos;
    }

    fw64_third_person_camera_set_collision_geometry_triangles_checked(cam, cam->character->scene->collision_geometry->last_raycast_test_count);
}

static void fw64_third_person_camera_update_cam_pos(fw64ThirdPersonCamera* cam, const Vec3* target_pos) {
    Vec3 forward = vec3_forward(), up = vec3_up(), look_at_target, cam_pos;

    // Calculate the camera's new offset position based on the rotation angles
    Quat q;
    quat_from_euler(&q, cam->rotation_x, cam->rotation_y, 0.0);

    quat_transform_vec3(&q, &up, &up);
    vec3_normalize(&up);

    quat_transform_vec3(&q, &forward, &forward);
    vec3_scale(&forward, cam->follow_dist, &forward);
    vec3_add(&forward, &cam->target_offset, &forward);
    vec3_add(target_pos, &forward, &cam_pos);
    vec3_add(target_pos, &cam->target_offset, &look_at_target);

    fw64_third_person_camera_resolve_world_collisions(cam, &look_at_target, &cam_pos);

    fw64_transform_look_at(&cam->camera->node->transform, &look_at_target, &up);
    fw64_node_update(cam->camera->node);
    fw64_camera_update_view_matrix(cam->camera);
}

static void fw64_third_person_camera_update_cam_pos_from_character_pos(fw64ThirdPersonCamera* cam) {
    fw64_third_person_camera_update_cam_pos(cam, &cam->character->node->transform.position);
}

static void fw64_third_person_camera_update_cam_from_animation(fw64ThirdPersonCamera* cam) {
    Vec3 ref_pos = vec3_zero();
    const fw64Transform* root_transform = fw64_animation_controller_get_joint_transform(&cam->anim_controller->skinned_mesh_instance->controller, cam->anim_controller->foot_reference_joint_index);
    matrix_transform_vec3(root_transform->world_matrix, &ref_pos);

    fw64_third_person_camera_update_cam_pos(cam, &ref_pos);
}

void fw64_third_person_camera_rotate(fw64ThirdPersonCamera* cam, float x, float y) {
    // this prevents funky camera view relative movement when at the extremes
    cam->rotation_x = fw64_clamp(cam->rotation_x + x, -89.0f, 89.0f);
    cam->rotation_y += y;
}

void fw64_third_person_camera_update(fw64ThirdPersonCamera* cam) {
    switch(cam->character->state) {
        case FW64_CHARACTER_STATE_LEDGE_CLIMB_UP:
        case FW64_CHARACTER_STATE_CLIMB_LADDER_EXIT:
        case FW64_CHARACTER_STATE_LADDER_ENTER_TOP:
            fw64_third_person_camera_update_cam_from_animation(cam);
            break;
        default:
            fw64_third_person_camera_update_cam_pos_from_character_pos(cam);
    }
}

void fw64_third_person_camera_reset(fw64ThirdPersonCamera* cam) {
    cam->follow_dist = FW64_THIRD_PERSON_CAMERA_DEFAULT_FOLLOW_DISTANCE;
    cam->rotation_x = 0.0f; // temp pick default value
    vec3_set_zero(&cam->target_offset);

    Vec3 target_back, forward = vec3_forward();
    fw64_transform_back(&cam->character->node->transform, &target_back);
    cam->rotation_y = (atan2f(target_back.z, target_back.x) - atan2f(forward.z, forward.x)) * (180.0f / M_PI);
}

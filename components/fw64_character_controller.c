#include "fw64_character_controller.h"

void fw64_character_controller_init(fw64CharacterController* controller, fw64Scene* scene, fw64Collider* collider) {
    controller->scene = scene;
    controller->collider = collider;
    vec3_zero(&controller->velocity);
}

void fw64_character_fixed_update(fw64CharacterController* controller, float time_delta) {
    Vec3 center, extents, delta;
    box_center(&controller->collider->bounding, &center);
    box_extents(&controller->collider->bounding, &extents);

    vec3_scale(&delta, &controller->velocity, time_delta);
    vec3_add(&center, &center, &delta);
    Box swept;
    box_set_center_extents(&swept, &center, &extents);
    box_encapsulate_box(&swept, &controller->collider->bounding);

    
}
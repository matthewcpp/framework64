#include "fw64_rotate_node.h"



void fw64_rotate_node_init(fw64RotateNode* component, fw64Node* node) {
    component->node = node;
    vec3_set(&component->axis, 0.0f, 1.0f, 0.0f);
    component->speed = FW64_ROTATE_NODE_DEFAULT_SPEED;
    component->direction = 1.0f;
    component->rotation = 0.0f;
    component->enabled = 1;
}

void fw64_rotate_node_update(fw64RotateNode* component, float time_delta) {
    if (!component->enabled) {
        return;
    }

    component->rotation += component->speed * time_delta;
    if (component->rotation >= 360.0f) {
        component->rotation -= 360.0f;
    }

    float radians = (M_PI / 180.0f) * component->rotation * component->direction;

    quat_set_axis_angle(&component->node->transform.rotation, component->axis.x, component->axis.y, component->axis.z , radians);
    fw64_node_update(component->node);
}

#pragma once

#include "framework64/node.h"

typedef struct {
    fw64Node* node;
    Vec3 axis;
    float speed;
    float rotation;
    float direction;
    int enabled;
} RotateNode;

void rotate_node_init(RotateNode* component, fw64Node* node);
void rotate_node_update(RotateNode* component, float time_delta);
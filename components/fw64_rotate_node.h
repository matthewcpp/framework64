#pragma once

#include "framework64/node.h"

#define FW64_ROTATE_NODE_DEFAULT_SPEED 75.0f

typedef struct {
    fw64Node* node;
    Vec3 axis;
    float speed;
    float rotation;
    float direction;
    int enabled;
} fw64RotateNode;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_rotate_node_init(fw64RotateNode* component, fw64Node* node);
void fw64_rotate_node_update(fw64RotateNode* component, float time_delta);

#ifdef __cplusplus
}
#endif

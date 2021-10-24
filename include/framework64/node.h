#pragma once

/** \file node.h */

#include "framework64/box.h"
#include "framework64/collider.h"
#include "framework64/camera.h"
#include "framework64/mesh.h"
#include "framework64/transform.h"

#include <limits.h>

typedef struct {
    fw64Transform transform;
    fw64Collider* collider;
    fw64Mesh* mesh;
    int type;
    uint32_t layer_mask;
} fw64Node;

#define FW64_NODE_UNSPECIFIED_TYPE INT32_MIN


#ifdef __cplusplus
extern "C" {
#endif

void fw64_node_init(fw64Node* node);
void fw64_node_set_collider(fw64Node* node, fw64Collider* collider);
void fw64_node_set_mesh(fw64Node* node, fw64Mesh* mesh);
void fw64_node_update(fw64Node* node);
void fw64_node_billboard(fw64Node* node, fw64Camera* camera);

#ifdef __cplusplus
}
#endif

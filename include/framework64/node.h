#pragma once

/** \file node.h */

#include "framework64/box.h"
#include "framework64/camera.h"
#include "framework64/mesh.h"
#include "framework64/transform.h"

#include <limits.h>

typedef struct {
    fw64Transform transform;
    Box bounding;
    fw64Mesh* mesh;
    int type;
    uint32_t layer_mask;
} fw64Node;

#define FW64_NODE_UNSPECIFIED_TYPE INT32_MIN


#ifdef __cplusplus
extern "C" {
#endif

void fw64_node_init(fw64Node* node, fw64Mesh* mesh);
void fw64_node_set_mesh(fw64Node* node, fw64Mesh* mesh);
void fw64_node_refresh(fw64Node* node);
void fw64_node_billboard(fw64Node* node, fw64Camera* camera);
void fw64_node_sphere(fw64Node* node, Vec3* center, float* radius);

#ifdef __cplusplus
}
#endif

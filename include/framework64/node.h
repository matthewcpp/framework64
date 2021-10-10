#pragma once

#include "framework64/box.h"
#include "framework64/camera.h"
#include "framework64/mesh.h"
#include "framework64/transform.h"

typedef struct {
    fw64Transform transform;
    Box bounding;
    fw64Mesh* mesh;
} fw64Node;


#ifdef __cplusplus
extern "C" {
#endif

void fw64_node_init(fw64Node* node, fw64Mesh* mesh);
void fw64_node_set_mesh(fw64Node* node, fw64Mesh* mesh);
void fw64_node_refresh(fw64Node* node);
void fw64_node_billboard(fw64Node* node, fw64Camera* camera);

#ifdef __cplusplus
}
#endif

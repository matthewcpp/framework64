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
    uint32_t layer_mask;
    void* data;
} fw64Node;

#define FW64_NODE_UNSPECIFIED_TYPE UINT32_MAX


#ifdef __cplusplus
extern "C" {
#endif

void fw64_node_init(fw64Node* node);
/** Associates the collider with this node.  
 * Sets the collider type to none (no collision) 
 */
void fw64_node_set_collider(fw64Node* node, fw64Collider* collider);

/** Associates the collider with this node.
 *  If the node has a mesh attached, will set the collider type to box for the current mesh
 */
void fw64_node_set_box_collider(fw64Node* node, fw64Collider* collider);

/** Associates the collider with this node.
 *  It will also set the collider to use the supplied collision mesh
 */
void fw64_node_set_mesh_collider(fw64Node* node, fw64Collider* collider, fw64CollisionMesh* collision_mesh);

/**
 * Sets the mesh associated with this node.
 * If the node has a box collider attached, and mesh is not null, it will update the collider to track the new mesh, otherwise the attached collider will be reset to type none.
 */
void fw64_node_set_mesh(fw64Node* node, fw64Mesh* mesh);
void fw64_node_update(fw64Node* node);
void fw64_node_billboard(fw64Node* node, fw64Camera* camera);

#ifdef __cplusplus
}
#endif

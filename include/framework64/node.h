#pragma once

/** \file node.h */

#include "framework64/box.h"
#include "framework64/collider.h"
#include "framework64/camera.h"
#include "framework64/transform.h"

#include <limits.h>

typedef struct fw64MeshInstance fw64MeshInstance;

typedef struct {
    fw64Transform transform;
    fw64Collider* collider;
    fw64MeshInstance* mesh_instance;
    void* data;
    uint32_t layer_mask;

    #ifdef FW64_PLATFORM_IS_64_BIT
    uint32_t _align;
    #endif

} fw64Node;

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

void fw64_node_update(fw64Node* node);
void fw64_node_billboard(fw64Node* node, fw64Camera* camera);

#ifdef __cplusplus
}
#endif

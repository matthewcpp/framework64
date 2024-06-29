#pragma once

/** \file node.h */

#include "framework64/box.h"
#include "framework64/collider.h"
#include "framework64/component.h"
#include "framework64/camera.h"
#include "framework64/mesh_instance.h"
#include "framework64/transform.h"

#include <limits.h>

struct fw64Node {
    fw64Transform transform;
    fw64Collider* collider;
    fw64MeshInstance* mesh_instance;
    fw64Component* components;
    uint32_t layer_mask;

    #ifdef FW64_PLATFORM_IS_64_BIT
    uint32_t _align;
    #endif

};

#ifdef __cplusplus
extern "C" {
#endif

void fw64_node_init(fw64Node* node);
void fw64_node_update(fw64Node* node);

void fw64_node_add_componet(fw64Node* node, fw64Component* component);
fw64Component* fw64_node_find_component(fw64Node* node, fw64ComponentTypeId component_type_id);

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

void fw64_node_billboard(fw64Node* node, fw64Camera* camera);

#ifdef __cplusplus
}
#endif

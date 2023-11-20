#pragma once

/** \file scene.h */

#include "framework64/data_io.h"
#include "framework64/material_bundle.h"
#include "framework64/mesh.h"
#include "framework64/node.h"
#include "framework64/quat.h"
#include "framework64/query.h"
#include "framework64/collision.h"
#include "framework64/renderer.h"
#include "framework64/vec3.h"

typedef struct fw64AssetDatabase fw64AssetDatabase;

typedef struct {
    uint32_t node_count;
    uint32_t mesh_count;
    uint32_t collider_count;
    uint32_t collision_mesh_count;
    uint32_t custom_bounding_box_count;
    uint32_t material_bundle_count;
} fw64SceneInfo;

struct fw64Scene {
    fw64SceneInfo info;
    fw64Node* nodes;
    fw64Collider* colliders;
    fw64Mesh** meshes;
    fw64CollisionMesh* collision_meshes;
    fw64MaterialBundle* material_bundle;
    fw64Allocator* allocator;
    Box bounding_box;
};

#ifdef __cplusplus
extern "C" {
#endif

fw64Scene* fw64_scene_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase* assets, fw64Allocator* allocator);
void fw64_scene_delete(fw64Scene* scene, fw64AssetDatabase* assets, fw64Allocator* allocator);

/** 
 * Gets the original bounding box for the scene as it was authored. 
 * This result may be inaccurate if nodes are transformed from their initial state
 * */
Box* fw64_scene_get_initial_bounds(fw64Scene* scene);

/**
 * Recalculates the scene's bounding based on the transform of all child nodes.
 */
void fw64_scene_update_bounding(fw64Scene* scene);

void fw64_scene_draw_all(fw64Scene* scene, fw64Renderer* renderer);
void fw64_scene_draw_frustrum(fw64Scene* scene, fw64Renderer* renderer, fw64Frustum* frustum);

fw64Mesh* fw64_scene_get_mesh(fw64Scene* scene, uint32_t index);
uint32_t fw64_scene_get_mesh_count(fw64Scene* scene);

fw64Node* fw64_scene_get_node(fw64Scene* scene, uint32_t index);
uint32_t fw64_scene_get_node_count(fw64Scene* scene);

fw64Allocator* fw64_scene_get_allocator(fw64Scene* scene);

uint32_t fw64_scene_find_nodes_with_layer_mask(fw64Scene* scene, uint32_t layer_mask, fw64Node** node_buffer, uint32_t buffer_size);

int fw64_scene_raycast(fw64Scene* scene, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit);
int fw64_scene_overlap_sphere(fw64Scene* scene, Vec3* center, float radius, uint32_t mask, fw64OverlapSphereQueryResult* result);
int fw64_scene_moving_sphere_intersection(fw64Scene* scene, Vec3* center, float radius, Vec3* velocity, uint32_t mask, fw64IntersectMovingSphereQuery* result);
int fw64_scene_moving_box_intersection(fw64Scene* scene, Box* box, Vec3* velocity, uint32_t mask, fw64IntersectMovingBoxQuery* result);

#ifdef __cplusplus
}
#endif

#pragma once

/** \file scene.h */

#include "framework64/collision.h"
#include "framework64/data_io.h"
#include "framework64/material_bundle.h"
#include "framework64/mesh.h"
#include "framework64/mesh_instance.h"
#include "framework64/node.h"
#include "framework64/quat.h"
#include "framework64/query.h"
#include "framework64/render_pass.h"
#include "framework64/skinned_mesh_instance.h"
#include "framework64/static_vector.h"
#include "framework64/vec3.h"

typedef struct fw64AssetDatabase fw64AssetDatabase;

typedef struct {
    uint16_t node_count;
    uint16_t extra_node_count;
    uint16_t mesh_count;
    uint16_t extra_mesh_count;
    uint32_t mesh_instance_count;
    uint32_t skinned_mesh_count;
    uint32_t skinned_mesh_instance_count;
    uint32_t collider_count;
    uint32_t collision_mesh_count;
    uint32_t custom_bounding_box_count;
    uint32_t material_bundle_count;
} fw64SceneInfo;

struct fw64Scene {
    fw64StaticVector meshes;
    fw64StaticVector mesh_instances;
    fw64StaticVector skinned_meshes;
    fw64StaticVector skinned_mesh_instances;
    fw64StaticVector colliders;
    fw64StaticVector collision_meshes;
    fw64StaticVector nodes;

    fw64MaterialBundle* material_bundle;
    fw64Allocator* allocator;
    fw64AssetDatabase* assets;
    Box bounding_box;
};

#ifdef __cplusplus
extern "C" {
#endif

fw64Scene* fw64_scene_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase* assets, fw64Allocator* allocator);
void fw64_scene_uninit(fw64Scene* scene);
void fw64_scene_delete(fw64Scene* scene);

void fw64_scene_info_init(fw64SceneInfo* info);
void fw64_scene_init(fw64Scene* scene, fw64SceneInfo* info, fw64AssetDatabase* assets, fw64Allocator* allocator);

/** 
 * Gets the original bounding box for the scene as it was authored. 
 * This result may be inaccurate if nodes are transformed from their initial state
 * */
Box* fw64_scene_get_initial_bounds(fw64Scene* scene);

/**
 * Recalculates the scene's bounding based on the transform of all child nodes.
 */
void fw64_scene_update_bounding(fw64Scene* scene);

void fw64_scene_update(fw64Scene* scene, float time_delta);

void fw64_scene_draw_all(fw64Scene* scene, fw64RenderPass* rendererpass, uint32_t layer_mask);
void fw64_scene_draw_frustrum(fw64Scene* scene, fw64RenderPass* rendererpass, fw64Frustum* frustum, uint32_t layer_mask);

fw64Mesh* fw64_scene_load_mesh_asset(fw64Scene* scene, fw64AssetId assetId);
int fw64_scene_insert_mesh(fw64Scene* scene, fw64Mesh* mesh);
fw64Mesh* fw64_scene_get_mesh(fw64Scene* scene, uint32_t index);
uint32_t fw64_scene_get_mesh_count(fw64Scene* scene);

fw64SkinnedMesh* fw64_scene_load_skinned_mesh_asset(fw64Scene* scene, fw64AssetId asset_id);
fw64SkinnedMesh* fw64_scene_get_skinned_mesh(fw64Scene* scene, uint32_t index);
uint32_t fw64_scene_get_skinned_mesh_count(fw64Scene* scene);

fw64Node* fw64_scene_create_node(fw64Scene* scene);
fw64Node* fw64_scene_get_node(fw64Scene* scene, uint32_t index);
uint32_t fw64_scene_get_node_count(fw64Scene* scene);

fw64MeshInstance* fw64_scene_create_mesh_instance(fw64Scene* scene, fw64Node* node, fw64Mesh* mesh);
fw64SkinnedMeshInstance* fw64_scene_create_skinned_mesh_instance(fw64Scene* scene, fw64Node* node, fw64SkinnedMesh* skinned_mesh, int initial_animation);
fw64Collider* fw64_scene_create_box_collider(fw64Scene* scene, fw64Node* node, Box* box);
fw64Allocator* fw64_scene_get_allocator(fw64Scene* scene);

uint32_t fw64_scene_find_nodes_with_layer_mask(fw64Scene* scene, uint32_t layer_mask, fw64Node** node_buffer, uint32_t buffer_size);

int fw64_scene_raycast(fw64Scene* scene, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit);
int fw64_scene_overlap_sphere(fw64Scene* scene, Vec3* center, float radius, uint32_t mask, fw64OverlapSphereQueryResult* result);
int fw64_scene_overlap_box(fw64Scene* scene, Box* box, uint32_t mask, fw64OverlapBoxQueryResult* result);
int fw64_scene_moving_sphere_intersection(fw64Scene* scene, Vec3* center, float radius, Vec3* velocity, uint32_t mask, fw64IntersectMovingSphereQuery* result);
int fw64_scene_moving_box_intersection(fw64Scene* scene, Box* box, Vec3* velocity, uint32_t mask, fw64IntersectMovingBoxQuery* result);

#define fw64_scene_get_mesh_instance_count(scene) (fw64_static_vector_size(&((scene)->mesh_instances)))
#define fw64_scene_get_mesh_instance(scene, index) ((fw64MeshInstance*)fw64_static_vector_get_item(&(scene)->mesh_instances, (index)))

#define fw64_scene_get_skinned_mesh_instance_count(scene) (fw64_static_vector_size(&((scene)->skinned_mesh_instances)))
#define fw64_scene_get_skinned_mesh_instance(scene, index) ((fw64SkinnedMeshInstance*)fw64_static_vector_get_item(&(scene)->skinned_mesh_instances, (index)))

#ifdef __cplusplus
}
#endif

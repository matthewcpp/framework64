#pragma once

#include "framework64/asset_database.h"
#include "framework64/mesh.h"
#include "framework64/node.h"
#include "framework64/quat.h"
#include "framework64/raycast.h"
#include "framework64/renderer.h"
#include "framework64/vec3.h"

typedef struct {
    Vec3 point;
    float distance;
} fw64RaycastHit;


typedef struct {
    fw64Node* node;
    Vec3 point;
} fw64OverlapSphereResult;

typedef struct {
    fw64OverlapSphereResult results[10];
    int count;
} fw64OverlapSphereQueryResult;

typedef struct fw64Scene fw64Scene;

#ifdef __cplusplus
extern "C" {
#endif

fw64Scene* fw64_scene_load(fw64AssetDatabase* assets, int handle);
void fw64_scene_delete(fw64Scene* scene);

void fw64_scene_draw_all(fw64Scene* scene, fw64Renderer* renderer);
fw64Mesh* fw64_scene_get_mesh(fw64Scene* scene, uint32_t index);
uint32_t fw64_scene_get_mesh_count(fw64Scene* scene);

fw64Node* fw64_scene_get_node(fw64Scene* scene, uint32_t index);
uint32_t fw64_scene_get_node_count(fw64Scene* scene);

int fw64_scene_raycast(fw64Scene* scene, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit);
int fw64_scene_overlap_sphere(fw64Scene* scene, Vec3* center, float radius, uint32_t mask, fw64OverlapSphereQueryResult* result);
#ifdef __cplusplus
}
#endif
#pragma once

#include "framework64/asset_database.h"
#include "framework64/mesh.h"
#include "framework64/quat.h"
#include "framework64/renderer.h"
#include "framework64/vec3.h"

typedef struct {
    uint32_t type;
    Vec3 position;
    Quat rotation;
    Vec3 scale;
} fw64SceneExtra;

typedef struct fw64Scene fw64Scene;

#ifdef __cplusplus
extern "C" {
#endif

fw64Scene* fw64_scene_load(fw64AssetDatabase* assets, int handle);
void fw64_scene_delete(fw64Scene* scene);

void fw64_scene_draw_all(fw64Scene* scene, fw64Renderer* renderer);
fw64Transform* fw64_scene_get_transform(fw64Scene* scene);
fw64Mesh* fw64_scene_get_mesh(fw64Scene* scene, uint32_t index);
uint32_t fw64_scene_get_extra_count(fw64Scene* scene);
fw64SceneExtra* fw64_scene_get_extras(fw64Scene* scene);
uint32_t fw64_scene_get_mesh_count(fw64Scene* scene);

#ifdef __cplusplus
}
#endif
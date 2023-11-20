#pragma once

/** \file scene.h */

#include "framework64/allocator.h"
#include "framework64/animation_data.h"
#include "framework64/data_io.h"

typedef struct fw64Mesh fw64Mesh;
typedef struct fw64AssetDatabase fw64AssetDatabase;

typedef struct {
    fw64Mesh* mesh;
    fw64AnimationData* animation_data;
} fw64SkinnedMesh;

#ifdef __cplusplus
extern "C" {
#endif

fw64SkinnedMesh* fw64_skinned_mesh_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase* assets, fw64Allocator* allocator);
void fw64_skinned_mesh_delete(fw64SkinnedMesh* skinned_mesh, fw64AssetDatabase* assets, fw64Allocator* allocator);

#ifdef __cplusplus
}
#endif
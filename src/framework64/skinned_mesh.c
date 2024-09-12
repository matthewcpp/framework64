#include "framework64/skinned_mesh.h"

#include "framework64/mesh.h"

fw64SkinnedMesh* fw64_skinned_mesh_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    fw64Mesh* mesh = fw64_mesh_load_from_datasource(assets, data_source, allocator);

    if (!mesh) {
        return NULL;
    }

    fw64AnimationData* animation_data = fw64_animation_data_load_from_datasource(data_source, allocator);

    if (!animation_data) {
        fw64_mesh_delete(mesh, assets, allocator);
        return NULL;
    }

    fw64SkinnedMesh* skinned_mesh = allocator->malloc(allocator, sizeof(fw64SkinnedMesh));
    skinned_mesh->mesh = mesh;
    skinned_mesh->animation_data = animation_data;

    return skinned_mesh;
}

void fw64_skinned_mesh_delete(fw64SkinnedMesh* skinned_mesh, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    fw64_mesh_delete(skinned_mesh->mesh, assets, allocator);
    fw64_animation_data_delete(skinned_mesh->animation_data, allocator);

    allocator->free(allocator, skinned_mesh);
}

#include "framework64/util/terrain.h"

#include "framework64/n64/filesystem.h"
#include "framework64/n64/loader.h"

#include <malloc.h>

struct fw64Terrain{
    fw64Transform transform;
    uint32_t mesh_count;
    fw64Mesh* meshes;
    fw64MeshResources* mesh_resources;
};

fw64Terrain* fw64_terrain_load(fw64AssetDatabase* database, uint32_t index) {
    int handle = fw64_filesystem_open(index);

    if (handle < 0)
        return NULL;

    fw64N64Loader loader;
    fw64_n64_loader_load_mesh_resources(&loader, handle);

    fw64Terrain* terrain = malloc(sizeof(fw64Terrain));
    fw64_transform_init(&terrain->transform);
    
    fw64_filesystem_read(&terrain->mesh_count, sizeof(uint32_t), 1, handle);
    terrain->meshes = malloc(terrain->mesh_count * sizeof(fw64Mesh));
    terrain->mesh_resources = loader.resources;

    for (uint32_t i = 0; i < terrain->mesh_count; i++) {
        fw64_n64_loader_load_mesh(&loader, terrain->meshes + i, handle);
    }

    fw64_filesystem_close(handle);

    return terrain;
}

void fw64_terrain_delete(fw64AssetDatabase* database, fw64Terrain* terrain) {
    for (uint32_t i = 0; i < terrain->mesh_count; i++) {
        fw64_n64_mesh_uninit(terrain->meshes + i);
    }

    free(terrain->meshes);
}


uint32_t fw64_terrain_get_mesh_count(fw64Terrain* terrain) {
    return terrain->mesh_count;
}

fw64Mesh* fw64_terrain_get_mesh(fw64Terrain* terrain, uint32_t index) {
    return terrain->meshes + index;
}

fw64Transform* fw64_terrain_get_transform(fw64Terrain* terrain) {
    return &terrain->transform;
}
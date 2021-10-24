#include "framework64/n64/scene.h"

#include "framework64/n64/filesystem.h"


#include <malloc.h>

fw64Scene* fw64_scene_load(fw64AssetDatabase* assets, int index) {
    int handle = fw64_filesystem_open(index);

    if (handle < 0)
        return NULL;

    fw64Scene* scene = malloc(sizeof(fw64Scene));
    fw64_transform_init(&scene->transform);

    fw64_filesystem_read(&scene->info, sizeof(fw64SceneInfo), 1, handle);


    fw64N64Loader loader;
    fw64_n64_loader_load_mesh_resources(&loader, handle);
    scene->mesh_resources = loader.resources;

    scene->meshes = malloc(scene->info.mesh_count * sizeof(fw64Mesh));

    for (uint32_t i = 0; i < scene->info.mesh_count; i++) {
        fw64_n64_loader_load_mesh(&loader, scene->meshes + i, handle);
    }

    scene->debug = fw64_filesystem_tell(handle);

    fw64_filesystem_close(handle);

    return scene;
}

void fw64_scene_delete(fw64Scene* scene) {
    for (uint32_t i = 0; i < scene->info.mesh_count; i++)
        fw64_n64_mesh_uninit(scene->meshes + i);

    fw64_n64_mesh_resources_delete(scene->mesh_resources);
    free(scene);
}

fw64Transform* fw64_scene_get_transform(fw64Scene* scene) {
    return &scene->transform;
}

fw64Mesh* fw64_scene_get_mesh(fw64Scene* scene, uint32_t index) {
    return scene->meshes + index;
}

uint32_t fw64_scene_get_mesh_count(fw64Scene* scene) {
    return scene->info.mesh_count;
}
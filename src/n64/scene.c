#include "framework64/n64/scene.h"

#include "framework64/n64/filesystem.h"


#include <malloc.h>

fw64Scene* fw64_scene_load(fw64AssetDatabase* assets, int index) {
    int handle = fw64_filesystem_open(index);

    if (handle < 0)
        return NULL;

    fw64Scene* scene = malloc(sizeof(fw64Scene));

    fw64_filesystem_read(&scene->info, sizeof(fw64SceneInfo), 1, handle);

    fw64N64Loader loader;
    fw64_n64_loader_load_mesh_resources(&loader, handle);
    scene->mesh_resources = loader.resources;

    if (scene->info.mesh_count > 0) {
        scene->meshes = malloc(scene->info.mesh_count * sizeof(fw64Mesh));

        for (uint32_t i = 0; i < scene->info.mesh_count; i++) {
            fw64_n64_loader_load_mesh(&loader, scene->meshes + i, handle);
        }
    }
    else {
        scene->meshes = NULL;
    }

    if (scene->info.collider_count > 0) {
        scene->colliders = malloc(scene->info.collider_count * sizeof(fw64Collider));
    }
    else {
        scene->colliders = NULL;
    }

    if (scene->info.node_count > 0) {
        scene->nodes = malloc(scene->info.node_count * sizeof(fw64Node));
        fw64_filesystem_read(scene->nodes, sizeof(fw64Node), scene->info.node_count, handle);

        // fixup node pointers
        int collider_index = 0;
        for (uint32_t i = 0; i < scene->info.node_count; i++) {
            fw64Node* node = scene->nodes + i;
            fw64_transform_update_matrix(&node->transform);

            // read the mesh index written to the node and grab the actual mesh
            fw64Mesh* mesh = NULL;
            uint32_t mesh_index = (uint32_t)node->mesh;
            node->mesh = NULL;

            if ( mesh_index != UINT32_MAX)
                mesh = scene->meshes + mesh_index;
            
            // assign a collider if the mesh needs one
            if (node->collider)
                fw64_node_set_collider(node, scene->colliders + (collider_index++));
            else
                node->collider = NULL;

            // set the mesh 
            fw64_node_set_mesh(node, mesh);
        }
    }
    else {
        scene->nodes = NULL;
    }


    fw64_filesystem_close(handle);

    return scene;
}

void fw64_scene_delete(fw64Scene* scene) {
    if (scene->meshes) {
        for (uint32_t i = 0; i < scene->info.mesh_count; i++)
            fw64_n64_mesh_uninit(scene->meshes + i);

        fw64_n64_mesh_resources_delete(scene->mesh_resources);

        free(scene->meshes);
    }

    if (scene->colliders)
        free(scene->colliders);
    
    if (scene->nodes) {
        free(scene->nodes);
    }

    free(scene);
}

fw64Mesh* fw64_scene_get_mesh(fw64Scene* scene, uint32_t index) {
    return scene->meshes + index;
}

uint32_t fw64_scene_get_mesh_count(fw64Scene* scene) {
    return scene->info.mesh_count;
}

fw64Node* fw64_scene_get_node(fw64Scene* scene, uint32_t index) {
    return scene->nodes + index;
}

uint32_t fw64_scene_get_node_count(fw64Scene* scene) {
    return scene->info.node_count;
}
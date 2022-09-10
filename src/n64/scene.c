#include "framework64/n64/scene.h"

#include "framework64/n64/filesystem.h"

#define FW64_N64_NODE_NO_MESH UINT32_MAX
#define FW64_N64_BOX_COLLIDER_USE_MESH_BOUNDING 256
#define FW64_N64_NODE_NO_COLLIDER UINT32_MAX

fw64Scene* fw64_scene_load(fw64AssetDatabase* assets, int index, fw64Allocator* allocator) {
    if (!allocator) allocator = fw64_default_allocator();

    (void)assets;
    int handle = fw64_filesystem_open(index);

    if (handle < 0)
        return NULL;

    fw64Scene* scene = allocator->malloc(allocator, sizeof(fw64Scene));
    scene->allocator = allocator;

    fw64_filesystem_read(&scene->info, sizeof(fw64SceneInfo), 1, handle);

    fw64N64Loader loader;
    fw64_n64_loader_init(&loader);
    fw64_n64_loader_load_mesh_resources(&loader, handle, allocator);
    scene->mesh_resources = loader.resources;

    if (scene->info.mesh_count > 0) {
        scene->meshes = allocator->malloc(allocator, scene->info.mesh_count * sizeof(fw64Mesh));

        for (uint32_t i = 0; i < scene->info.mesh_count; i++) {
            fw64Mesh* mesh = scene->meshes + i;
            fw64_n64_loader_load_mesh(&loader, mesh, handle, allocator);
            mesh->resources = NULL; // scene will hold reference to all resources and free them during delete
        }
    }
    else {
        scene->meshes = NULL;
    }

    if (scene->info.node_count > 0) {
        scene->colliders = allocator->malloc(allocator, scene->info.collider_count * sizeof(fw64Collider));
    }
    else {
        scene->colliders = NULL;
    }

    if (scene->info.collision_mesh_count > 0) {
        scene->collision_meshes = allocator->malloc(allocator, scene->info.collision_mesh_count * sizeof(fw64CollisionMesh));

        for(uint32_t i = 0; i < scene->info.collision_mesh_count; i++) {
            fw64CollisionMesh* collision_mesh = scene->collision_meshes + i;

            // read the header in order to determine data size
            fw64_filesystem_read(collision_mesh, sizeof(fw64CollisionMesh), 1, handle);

            uint32_t point_data_size = collision_mesh->point_count * sizeof(Vec3);
            uint32_t element_data_size = collision_mesh->element_count * sizeof(uint16_t);
            char* data_buffer = allocator->malloc(allocator, point_data_size + element_data_size);

            //read data buffer and update pointers
            fw64_filesystem_read(data_buffer, 1, point_data_size + element_data_size, handle);
            collision_mesh->points = (Vec3*)data_buffer;
            collision_mesh->elements = (uint16_t*)(data_buffer + point_data_size);
        }
    }
    else {
        scene->collision_meshes = NULL;
    }

    if (scene->info.node_count > 0) {
        box_invalidate(&scene->bounding_box);

        scene->nodes = allocator->memalign(allocator, 8, scene->info.node_count * sizeof(fw64Node));
        fw64_filesystem_read(scene->nodes, sizeof(fw64Node), scene->info.node_count, handle);

        Box* custom_bounding_boxes = NULL;
        if (scene->info.custom_bounding_box_count) {
            custom_bounding_boxes = allocator->malloc(allocator, sizeof(Box) * scene->info.custom_bounding_box_count);
            fw64_filesystem_read(custom_bounding_boxes, sizeof(Box), scene->info.custom_bounding_box_count, handle);
        }

        uint32_t collider_index = 0;

        for (uint32_t i = 0; i < scene->info.node_count; i++) {
            fw64Node* node = scene->nodes + i;
            fw64_transform_update_matrix(&node->transform);

            // read the mesh index written to the node
            uint32_t mesh_index = (uint32_t)node->mesh;
            node->mesh = NULL;

            if (mesh_index != FW64_N64_NODE_NO_MESH) {
                fw64_node_set_mesh(node, scene->meshes + mesh_index);
            }

            // read the collider info written to the node
            uint32_t collider_value = (uint32_t)node->collider;
            uint32_t collider_type = collider_value & 0xFFFF;
            uint32_t collision_mesh_index = collider_value >> 16;
            node->collider = NULL;
            
            if (collider_value != FW64_N64_NODE_NO_COLLIDER) {
                fw64_node_set_collider(node, scene->colliders + collider_index);

                if (collider_type == FW64_COLLIDER_BOX) {
                    if (collision_mesh_index == FW64_N64_BOX_COLLIDER_USE_MESH_BOUNDING)
                        fw64_collider_set_type_box(node->collider, &node->mesh->info.bounding_box);
                    else
                        fw64_collider_set_type_box(node->collider, custom_bounding_boxes + collision_mesh_index);
                }
                else if (collider_type == FW64_COLLIDER_MESH) {
                    fw64_collider_set_type_mesh(node->collider, scene->collision_meshes + collision_mesh_index);
                }

                collider_index += 1;
                box_encapsulate_box(&scene->bounding_box, &node->collider->bounding);
            }
        }

        if (custom_bounding_boxes) {
            allocator->free(allocator, custom_bounding_boxes);
        }
    }
    else {
        scene->nodes = NULL;
        vec3_set_all(&scene->bounding_box.min, -1.0f);
        vec3_set_all(&scene->bounding_box.max, 1.0f);
    }


    fw64_filesystem_close(handle);
    fw64_n64_loader_uninit(&loader);

    return scene;
}

void fw64_scene_update_bounding(fw64Scene* scene) {
    box_invalidate(&scene->bounding_box);

    for (uint32_t i = 0; i < scene->info.node_count; i++) {
        fw64Node* node = &scene->nodes[i];

        if (!node->collider || node->collider->type == FW64_COLLIDER_NONE) {
            continue;
        }

        box_encapsulate_box(&scene->bounding_box, &node->collider->bounding);
    }
}

void fw64_scene_delete(fw64AssetDatabase * assets, fw64Scene* scene, fw64Allocator* allocator) {
    if (!allocator) allocator = fw64_default_allocator();

    (void)assets;
    if (scene->meshes) {
        for (uint32_t i = 0; i < scene->info.mesh_count; i++)
            fw64_n64_mesh_uninit(scene->meshes + i, allocator);

        allocator->free(allocator, scene->meshes);
    }

    if (scene->mesh_resources) {
        fw64_n64_mesh_resources_delete(scene->mesh_resources, allocator);
    }

    if (scene->collision_meshes) {
        for (uint32_t i = 0; i < scene->info.collision_mesh_count; i++) {
            fw64CollisionMesh* collision_mesh = scene->collision_meshes + i;
            // note the chunk of data for the mesh collider is allocated together so just need to free the start
            allocator->free(allocator, collision_mesh->points);
        }

        allocator->free(allocator, scene->collision_meshes);
    }

    if (scene->colliders)
        allocator->free(allocator, scene->colliders);
    
    if (scene->nodes) {
        allocator->free(allocator, scene->nodes);
    }

    allocator->free(allocator, scene);
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

Box* fw64_scene_get_initial_bounds(fw64Scene* scene) {
    return &scene->bounding_box;
}

fw64Allocator* fw64_scene_get_allocator(fw64Scene* scene) {
    return scene->allocator;
}
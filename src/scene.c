#include "framework64/scene.h"

#include <string.h>

#define FW64_N64_NODE_NO_MESH UINT32_MAX
#define FW64_N64_BOX_COLLIDER_USE_MESH_BOUNDING 256
#define FW64_N64_NODE_NO_COLLIDER UINT32_MAX

void fw64_scene_info_init(fw64SceneInfo* info) {
    memset(info, 0, sizeof(fw64SceneInfo));
}

void fw64_scene_init(fw64Scene* scene, fw64SceneInfo* info, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    if (info) {
        scene->info = *info;
    } else {
        fw64_scene_info_init(&scene->info);
    }

    scene->allocator = allocator;
    scene->assets = assets;
    scene->material_bundle = NULL; // TODO should this be supported?

    if (scene->info.mesh_count > 0) {
        scene->meshes = allocator->malloc(allocator, scene->info.mesh_count * sizeof(fw64Mesh*));
        memset(scene->meshes, 0, scene->info.mesh_count * sizeof(fw64Mesh*));
    } else {
        scene->meshes = NULL;
    }

    if (scene->info.collider_count > 0) {
        scene->colliders = allocator->malloc(allocator, scene->info.collider_count * sizeof(fw64Collider));
        memset(scene->colliders, 0, scene->info.collider_count * sizeof(fw64Collider));
    } else {
        scene->colliders = NULL;
    }

    if (scene->info.collision_mesh_count > 0) {
        scene->collision_meshes = allocator->malloc(allocator, scene->info.collision_mesh_count * sizeof(fw64CollisionMesh));
        memset(scene->collision_meshes, 0, scene->info.collision_mesh_count * sizeof(fw64CollisionMesh));
    } else {
        scene->collision_meshes = NULL;
    }

    if (scene->info.node_count > 0) {
        scene->nodes = allocator->memalign(allocator, 8, scene->info.node_count * sizeof(fw64Node));

        for (uint32_t i = 0; i < scene->info.node_count; i++) {
            fw64_node_init(scene->nodes + i);
        }
    } else {
        scene->nodes = NULL;
    }
}

fw64Scene* fw64_scene_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    fw64SceneInfo scene_info;
    fw64_data_source_read(data_source, &scene_info, sizeof(fw64SceneInfo), 1);

    fw64MaterialBundle* material_bundle = NULL;
    
    if (scene_info.material_bundle_count == 1) {
        material_bundle = fw64_material_bundle_load_from_datasource(data_source, assets, allocator);
    }

    if (!material_bundle) {
        return NULL;
    }

    fw64Scene* scene = allocator->malloc(allocator, sizeof(fw64Scene));
    memcpy(&scene->info, &scene_info, sizeof(fw64SceneInfo));
    scene->allocator = allocator;
    scene->assets = assets;
    scene->material_bundle = material_bundle;

    if (scene->info.mesh_count > 0) {
        scene->meshes = allocator->malloc(allocator, scene->info.mesh_count * sizeof(fw64Mesh*));

        for (uint32_t i = 0; i < scene->info.mesh_count; i++) {
            scene->meshes[i] = fw64_mesh_load_from_datasource_with_bundle(assets, data_source, scene->material_bundle, allocator);
        }
    }
    else {
        scene->meshes = NULL;
    }

    if (scene->info.collider_count > 0) {
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
            fw64_data_source_read(data_source, collision_mesh, sizeof(fw64CollisionMesh), 1);

            uint32_t point_data_size = collision_mesh->point_count * sizeof(Vec3);
            uint32_t element_data_size = collision_mesh->element_count * sizeof(uint16_t);
            char* data_buffer = allocator->malloc(allocator, point_data_size + element_data_size);

            //read data buffer and update pointers
            fw64_data_source_read(data_source, data_buffer, 1, point_data_size + element_data_size);
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
        fw64_data_source_read(data_source, scene->nodes, sizeof(fw64Node), scene->info.node_count);

        Box* custom_bounding_boxes = NULL;
        if (scene->info.custom_bounding_box_count) {
            custom_bounding_boxes = allocator->malloc(allocator, sizeof(Box) * scene->info.custom_bounding_box_count);
            fw64_data_source_read(data_source, custom_bounding_boxes, sizeof(Box), scene->info.custom_bounding_box_count);
        }

        uint32_t collider_index = 0;

        for (uint32_t i = 0; i < scene->info.node_count; i++) {
            fw64Node* node = scene->nodes + i;
            fw64_transform_update_matrix(&node->transform);

            // read the mesh index written to the node
            uintptr_t mesh_index = (uintptr_t)node->mesh;
            node->mesh = NULL;

            if (mesh_index != FW64_N64_NODE_NO_MESH) {
                fw64_node_set_mesh(node, scene->meshes[mesh_index]);
            }

            // read the collider info written to the node
            uintptr_t collider_value = (uintptr_t)node->collider;
            uintptr_t collider_type = collider_value & 0xFFFF;
            uintptr_t collision_mesh_index = collider_value >> 16;
            node->collider = NULL;
            
            if (collider_value != FW64_N64_NODE_NO_COLLIDER) {
                fw64_node_set_collider(node, scene->colliders + collider_index);

                if (collider_type == FW64_COLLIDER_BOX) {
                    if (collision_mesh_index == FW64_N64_BOX_COLLIDER_USE_MESH_BOUNDING) {
                        Box bounding_box = fw64_mesh_get_bounding_box(node->mesh);
                        fw64_collider_set_type_box(node->collider, &bounding_box);
                    }
                    else {
                        fw64_collider_set_type_box(node->collider, custom_bounding_boxes + collision_mesh_index);
                    }
                }
                else if (collider_type == FW64_COLLIDER_MESH) {
                    fw64_collider_set_type_mesh(node->collider, scene->collision_meshes + collision_mesh_index);
                }
                else if (collider_type == FW64_COLLIDER_NONE) {
                    fw64_collider_set_type_none(node->collider);
                }

                collider_index += 1;

                if (collider_type != FW64_COLLIDER_NONE)
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

    return scene;
}

void fw64_scene_uninit(fw64Scene* scene) {
if (scene->meshes) {
        for (uint32_t i = 0; i < scene->info.mesh_count; i++)
            fw64_mesh_delete(scene->meshes[i], scene->assets, scene->allocator);

        scene->allocator->free(scene->allocator, scene->meshes);
    }

    if (scene->material_bundle) {
        fw64_material_bundle_delete(scene->material_bundle, scene->allocator);
    }

    if (scene->collision_meshes) {
        for (uint32_t i = 0; i < scene->info.collision_mesh_count; i++) {
            fw64CollisionMesh* collision_mesh = scene->collision_meshes + i;
            // note the chunk of data for the mesh collider is allocated together so just need to free the start
            scene->allocator->free(scene->allocator, collision_mesh->points);
        }

        scene->allocator->free(scene->allocator, scene->collision_meshes);
    }

    if (scene->colliders)
        scene->allocator->free(scene->allocator, scene->colliders);

    if (scene->nodes) {
        scene->allocator->free(scene->allocator, scene->nodes);
    }
}

void fw64_scene_delete(fw64Scene* scene) {
    fw64_scene_uninit(scene);
    scene->allocator->free(scene->allocator, scene);
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

void fw64_scene_draw_all(fw64Scene* scene, fw64Renderer* renderer) {
    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (uint32_t i = 0 ; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->mesh)
            continue;

        fw64_renderer_draw_static_mesh(renderer, &node->transform, node->mesh);
    }
}

void fw64_scene_draw_frustrum(fw64Scene* scene, fw64Renderer* renderer, fw64Frustum* frustum) {
    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (uint32_t i = 0 ; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->mesh || !node->collider) // TODO: requiring collider may not be the most ideal setup
            continue;

        if (fw64_frustum_intersects_box(frustum, &node->collider->bounding)) {
            fw64_renderer_draw_static_mesh(renderer, &node->transform, node->mesh);
        }
    }
}

int fw64_scene_raycast(fw64Scene* scene, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit) {
    uint32_t node_count = fw64_scene_get_node_count(scene);
    hit->distance = FLT_MAX;
    hit->node = NULL;

    Vec3 hit_pos;
    float dist;

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->collider || !(mask & node->layer_mask))
            continue;

        if (fw64_collider_test_ray(node->collider, origin, direction, &hit_pos, &dist)) {
            if (dist > hit->distance) continue;

            hit->node = node;
            hit->point = hit_pos;
            hit->distance = dist;
        }
    }

    return hit->node != NULL;
}

int fw64_scene_overlap_sphere(fw64Scene* scene, Vec3* center, float radius, uint32_t mask, fw64OverlapSphereQueryResult* result) {
    result->count = 0;
    fw64OverlapSphereResult* sphere_hit = &result->results[0];

    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->collider || !(node->layer_mask & mask))
            continue;

        if (fw64_collider_test_sphere(node->collider, center, radius, &sphere_hit->point)) {
            sphere_hit->node = node;
            sphere_hit = &result->results[++result->count];
        }

        // todo: probably a better way to handle this?
        if (result->count == Fw64_COLLISION_QUERY_RESULT_SIZE)
            break;
    }

    return result->count;
}

int fw64_scene_moving_sphere_intersection(fw64Scene* scene, Vec3* center, float radius, Vec3* velocity,
                                          uint32_t mask, fw64IntersectMovingSphereQuery* result) {
    IntersectMovingSphereResult* sphere_intersect = &result->results[result->count];

    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->collider || !(node->layer_mask & mask))
            continue;

        if(fw64_collision_test_moving_sphere_box(center, radius, velocity, &node->collider->bounding,
                                                 &sphere_intersect->point, &sphere_intersect->distance)) {
            sphere_intersect->node = node;
            sphere_intersect = &result->results[++result->count];
        }

        if (result->count >= Fw64_COLLISION_QUERY_RESULT_SIZE)
            break;
    }

    return result->count;
}

int fw64_scene_moving_box_intersection(fw64Scene* scene, Box* box, Vec3* velocity, uint32_t mask, fw64IntersectMovingBoxQuery* result) {
    Vec3 static_vel = {0.0f, 0.0f, 0.0f};
    IntersectMovingBoxResult* box_intersect = &result->results[result->count];

    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->collider || !(node->layer_mask & mask))
            continue;

        if (fw64_collision_test_moving_boxes(&node->collider->bounding, &static_vel, box, velocity, &box_intersect->tfirst, &box_intersect->tlast)) {
            box_intersect->node = node;
            box_intersect = &result->results[++result->count];
        }

        // todo: probably a better way to handle this?
        if (result->count == Fw64_COLLISION_QUERY_RESULT_SIZE)
            break;
    }

    return result->count;
}

uint32_t fw64_scene_find_nodes_with_layer_mask(fw64Scene* scene, uint32_t layer_mask, fw64Node** node_buffer, uint32_t buffer_size) {
    uint32_t buffer_index = 0;

        uint32_t node_count = fw64_scene_get_node_count(scene);

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (node->layer_mask & layer_mask) {
            node_buffer[buffer_index++] = node;

            if (buffer_index == buffer_size)
                return buffer_index;
        }
    }

    return buffer_index;
}

fw64Mesh* fw64_scene_load_mesh_asset(fw64Scene* scene, fw64AssetId asset_id, uint32_t mesh_index) {
    fw64Mesh* mesh = fw64_assets_load_mesh(scene->assets, asset_id, scene->allocator);

    if (mesh) {
        if (scene->meshes[mesh_index]) {
            fw64_mesh_delete(scene->meshes[mesh_index], scene->assets, scene->allocator);
        }

        scene->meshes[mesh_index] = mesh;
    }

    return mesh;
}

fw64Mesh* fw64_scene_get_mesh(fw64Scene* scene, uint32_t index) {
    return scene->meshes[index];
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

fw64Collider* fw64_scene_get_collider(fw64Scene* scene, uint32_t index) {
    return &scene->colliders[index];
}

uint32_t fw64_scene_get_collider_count(fw64Scene* scene) {
    return scene->info.collider_count;
}

Box* fw64_scene_get_initial_bounds(fw64Scene* scene) {
    return &scene->bounding_box;
}

fw64Allocator* fw64_scene_get_allocator(fw64Scene* scene) {
    return scene->allocator;
}

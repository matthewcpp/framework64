#include "framework64/scene.h"

#include <string.h>

#define FW64_N64_NODE_NO_MESH UINT32_MAX
#define FW64_N64_BOX_COLLIDER_USE_MESH_BOUNDING 256

#define FW64_N64_NODE_NO_COLLIDER UINT32_MAX
#define FW64_N64_NODE_COLLIDER_FROM_TRANSFORM (UINT32_MAX - 1)

void fw64_scene_info_init(fw64SceneInfo* info) {
    memset(info, 0, sizeof(fw64SceneInfo));
}

static void init_static_vectors(fw64Scene* scene, fw64SceneInfo* info) {
    fw64_static_vector_init(&scene->meshes, sizeof(fw64Mesh*), info->mesh_count + info->extra_mesh_count, scene->allocator);
    fw64_static_vector_init(&scene->skinned_meshes, sizeof(fw64SkinnedMesh*), info->skinned_mesh_count, scene->allocator);
    fw64_static_vector_init(&scene->mesh_instances, sizeof(fw64MeshInstance), info->mesh_instance_count, scene->allocator);
    fw64_static_vector_init(&scene->skinned_mesh_instances, sizeof(fw64SkinnedMeshInstance), info->skinned_mesh_instance_count, scene->allocator);
    fw64_static_vector_init(&scene->colliders, sizeof(fw64Collider), info->collider_count, scene->allocator);
    fw64_static_vector_init(&scene->collision_meshes, sizeof(fw64CollisionMesh), info->collision_mesh_count, scene->allocator);
    fw64_static_vector_init(&scene->nodes, sizeof(fw64Node), info->node_count + info->extra_node_count, scene->allocator);
}

void fw64_scene_init(fw64Scene* scene, fw64SceneInfo* info, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    scene->allocator = allocator;
    scene->assets = assets;
    scene->material_bundle = NULL; // TODO should this be supported?

    init_static_vectors(scene, info);
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

    fw64Scene* scene = fw64_allocator_malloc(allocator, sizeof(fw64Scene));
    scene->allocator = allocator;
    scene->assets = assets;
    scene->material_bundle = material_bundle;

    init_static_vectors(scene, &scene_info);

    for (uint32_t i = 0; i < scene_info.mesh_count; i++) {
        fw64Mesh* mesh = fw64_mesh_load_from_datasource_with_bundle(assets, data_source, scene->material_bundle, allocator);
        fw64_static_vector_push_back(&scene->meshes, &mesh);
    }

    for(uint32_t i = 0; i < scene_info.collision_mesh_count; i++) {
        fw64CollisionMesh* collision_mesh = fw64_static_vector_alloc_back(&scene->collision_meshes);
        fw64_collision_mesh_load_from_datasource(collision_mesh, data_source, allocator);
    }

    if (scene_info.node_count > 0) {
        box_invalidate(&scene->bounding_box);

        fw64_static_vector_resize(&scene->nodes, scene_info.node_count);
        fw64_data_source_read(data_source, scene->nodes.data, sizeof(fw64Node), scene_info.node_count);
    } else {
        vec3_set_all(&scene->bounding_box.min, -1.0f);
        vec3_set_all(&scene->bounding_box.max, 1.0f);
    }

    // this is read last so that in the case of bump allocator the space can be re-used.
    Box* custom_bounding_boxes = NULL;
    if (scene_info.custom_bounding_box_count) {
        custom_bounding_boxes = fw64_allocator_malloc(allocator, sizeof(Box) * scene_info.custom_bounding_box_count);
        fw64_data_source_read(data_source, custom_bounding_boxes, sizeof(Box), scene_info.custom_bounding_box_count);
    }

    for (uint32_t i = 0; i < scene_info.node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);
        fw64_transform_update_matrix(&node->transform);

        // read the mesh index written to the node
        uintptr_t mesh_index = (uintptr_t)node->mesh_instance;

        if (mesh_index != FW64_N64_NODE_NO_MESH) {
            fw64Mesh* mesh = fw64_scene_get_mesh(scene, (uint32_t)mesh_index);
            fw64_scene_create_mesh_instance(scene, node, mesh);
        } 
        else {
            node->mesh_instance = NULL;
        }

        // read the collider info written to the node
        uintptr_t collider_value = (uintptr_t)node->collider;
        uintptr_t collider_type = collider_value & 0xFFFF;
        uintptr_t collision_mesh_index = collider_value >> 16;
        node->collider = NULL;
        
        if (collider_value != FW64_N64_NODE_NO_COLLIDER) {
            fw64Collider* collider = fw64_static_vector_alloc_back(&scene->colliders);

            if (collider_type == FW64_COLLIDER_BOX) {
                if (collision_mesh_index == FW64_N64_BOX_COLLIDER_USE_MESH_BOUNDING) {
                    Box bounding_box = fw64_mesh_get_bounding_box(node->mesh_instance->mesh);
                    fw64_collider_init_box(collider, node, &bounding_box);
                }
                else {
                    fw64_collider_init_box(collider, node, custom_bounding_boxes + collision_mesh_index);
                }
            }
            else if (collider_type == FW64_COLLIDER_COLLISION_MESH) {
                fw64CollisionMesh* collision_mesh = fw64_static_vector_get_item(&scene->collision_meshes, (uint32_t)collision_mesh_index);
                fw64_collider_init_collision_mesh(collider, node, collision_mesh);
            }

            box_encapsulate_box(&scene->bounding_box, &node->collider->bounding);
        }
        else {
            node->collider = NULL;
        }
    }
    

    if (custom_bounding_boxes) {
        fw64_allocator_free(allocator, custom_bounding_boxes);
    }

    return scene;
}

void fw64_scene_uninit(fw64Scene* scene) {
    if (scene->material_bundle) {
        fw64_material_bundle_delete(scene->material_bundle, scene->allocator);
    }

    for (uint32_t i = 0; i < fw64_scene_get_mesh_count(scene); i++) {
        fw64_mesh_delete(fw64_scene_get_mesh(scene, i), scene->assets, scene->allocator);
    }

    for (uint32_t i = 0; i < fw64_scene_get_skinned_mesh_count(scene); i++) {
        fw64_skinned_mesh_delete(fw64_scene_get_skinned_mesh(scene, i), scene->assets, scene->allocator);
    }

    for (uint32_t i = 0; i < fw64_static_vector_size(&scene->collision_meshes); i++) {
        fw64_collision_mesh_uninit(fw64_static_vector_get_item(&scene->collision_meshes, i), scene->allocator);
    }

    fw64_static_vector_uninit(&scene->meshes, scene->allocator);
    fw64_static_vector_uninit(&scene->skinned_meshes, scene->allocator);
    fw64_static_vector_uninit(&scene->mesh_instances, scene->allocator);
    fw64_static_vector_uninit(&scene->skinned_mesh_instances, scene->allocator);
    fw64_static_vector_uninit(&scene->colliders, scene->allocator);
    fw64_static_vector_uninit(&scene->collision_meshes, scene->allocator);
    fw64_static_vector_uninit(&scene->nodes, scene->allocator);
}

void fw64_scene_delete(fw64Scene* scene) {
    fw64_scene_uninit(scene);
    fw64_allocator_free(scene->allocator, scene);
}

void fw64_scene_update_bounding(fw64Scene* scene) {
    box_invalidate(&scene->bounding_box);

    for (uint32_t i = 0; i < fw64_scene_get_node_count(scene); i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (node->collider) {
            box_encapsulate_box(&scene->bounding_box, &node->collider->bounding);
        }
    }
}

void fw64_scene_update(fw64Scene* scene, float time_delta) {
    for (uint32_t i = 0; i < fw64_scene_get_skinned_mesh_instance_count(scene); i++) {
        fw64_skinned_mesh_instance_update(fw64_scene_get_skinned_mesh_instance(scene, i), time_delta);
    }
}

void fw64_scene_draw_all(fw64Scene* scene, fw64RenderPass* rendererpass) {
    for (uint32_t i = 0 ; i < fw64_scene_get_mesh_instance_count(scene); i++) {
        fw64_renderpass_draw_static_mesh(rendererpass, fw64_scene_get_mesh_instance(scene,i));
    }

    for (uint32_t i = 0 ; i < fw64_scene_get_skinned_mesh_instance_count(scene); i++) {
        fw64_renderpass_draw_skinned_mesh(rendererpass, fw64_scene_get_skinned_mesh_instance(scene, i));
    }
}

void fw64_scene_draw_frustrum(fw64Scene* scene, fw64RenderPass* rendererpass, fw64Frustum* frustum, uint32_t layer_mask) {
    for (uint32_t i = 0 ; i < fw64_scene_get_mesh_instance_count(scene); i++) {
        fw64MeshInstance* mesh_instance = fw64_scene_get_mesh_instance(scene, i);

        if (!(mesh_instance->node->layer_mask & layer_mask)) {
            continue;
        }

        if (fw64_frustum_intersects_box(frustum, &mesh_instance->render_bounds)) {
            fw64_renderpass_draw_static_mesh(rendererpass, mesh_instance);
        }
    }

    for (uint32_t i = 0; i < fw64_scene_get_skinned_mesh_instance_count(scene); i++) {
        fw64SkinnedMeshInstance* skinned_mesh_instance = fw64_scene_get_skinned_mesh_instance(scene, i);

        if (!(skinned_mesh_instance->mesh_instance.node->layer_mask & layer_mask)) {
            continue;
        }

        if (fw64_frustum_intersects_box(frustum, &skinned_mesh_instance->mesh_instance.render_bounds)) {
            fw64_renderpass_draw_skinned_mesh(rendererpass, skinned_mesh_instance);
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

        if (!node->collider || !(mask & node->layer_mask) || !fw64_collider_is_active(node->collider))
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

        if (!node->collider || !(node->layer_mask & mask) || !fw64_collider_is_active(node->collider))
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

int fw64_scene_overlap_box(fw64Scene* scene, Box* box, uint32_t mask, fw64OverlapBoxQueryResult* result) {
    result->count = 0;

    uint32_t node_count = fw64_scene_get_node_count(scene);

    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(scene, i);

        if (!node->collider || !(node->layer_mask & mask) || !fw64_collider_is_active(node->collider))
            continue;

        if (fw64_collider_test_box(node->collider, box)) {
            result->colliders[result->count++] = node->collider;
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

        if (!node->collider || !(node->layer_mask & mask) || !fw64_collider_is_active(node->collider))
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

        if (!node->collider || !(node->layer_mask & mask) || !fw64_collider_is_active(node->collider))
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

fw64Mesh* fw64_scene_load_mesh_asset(fw64Scene* scene, fw64AssetId asset_id) {
    fw64Mesh* mesh = fw64_assets_load_mesh(scene->assets, asset_id, scene->allocator);
    fw64_static_vector_push_back(&scene->meshes, &mesh);

    return mesh;
}

int fw64_scene_insert_mesh(fw64Scene* scene, fw64Mesh* mesh) {
    return fw64_static_vector_push_back(&scene->meshes, &mesh);
}


fw64Mesh* fw64_scene_get_mesh(fw64Scene* scene, uint32_t index) {
    return *((fw64Mesh**)fw64_static_vector_get_item(&scene->meshes, index));
}

uint32_t fw64_scene_get_mesh_count(fw64Scene* scene) {
    return fw64_static_vector_size(&scene->meshes);
}

fw64SkinnedMesh* fw64_scene_load_skinned_mesh_asset(fw64Scene* scene, fw64AssetId asset_id) {
    fw64SkinnedMesh* skinned_mesh = fw64_assets_load_skinned_mesh(scene->assets, asset_id, scene->allocator);

    if (skinned_mesh) {
        fw64_static_vector_push_back(&scene->skinned_meshes, &skinned_mesh);
    }

    return skinned_mesh;
}

fw64SkinnedMesh* fw64_scene_get_skinned_mesh(fw64Scene* scene, uint32_t index) {
    return fw64_static_vector_get_item(&scene->skinned_meshes, index);
}

uint32_t fw64_scene_get_skinned_mesh_count(fw64Scene* scene) {
    return fw64_static_vector_size(&scene->skinned_meshes);
}

fw64Node* fw64_scene_create_node(fw64Scene* scene) {
    fw64Node* node = fw64_static_vector_alloc_back(&scene->nodes);

    if (node) {
        fw64_node_init(node);
    }

    return node;
}

fw64Node* fw64_scene_get_node(fw64Scene* scene, uint32_t index) {
    return fw64_static_vector_get_item(&scene->nodes, index);
}

uint32_t fw64_scene_get_node_count(fw64Scene* scene) {
    return fw64_static_vector_size(&scene->nodes);
}

fw64MeshInstance* fw64_scene_create_mesh_instance(fw64Scene* scene, fw64Node* node, fw64Mesh* mesh) {
    fw64MeshInstance* mesh_instance = (fw64MeshInstance*)fw64_static_vector_alloc_back(&(scene)->mesh_instances);

    if (mesh_instance) {
        fw64_mesh_instance_init(mesh_instance, node, mesh);
    }

    return mesh_instance;
}

fw64SkinnedMeshInstance* fw64_scene_create_skinned_mesh_instance(fw64Scene* scene, fw64Node* node, fw64SkinnedMesh* skinned_mesh, int initial_animation) {
    fw64SkinnedMeshInstance* skinned_mesh_instance = (fw64SkinnedMeshInstance*)fw64_static_vector_alloc_back(&scene->skinned_mesh_instances);

    if (skinned_mesh_instance) {
        fw64_skinned_mesh_instance_init(skinned_mesh_instance, node, skinned_mesh, initial_animation, scene->allocator);
    }

    return skinned_mesh_instance;
}

fw64Collider* fw64_scene_create_box_collider(fw64Scene* scene, fw64Node* node, Box* box) {
    fw64Collider* collider = (fw64Collider*)fw64_static_vector_alloc_back(&scene->colliders);

    if (collider) {
        fw64_collider_init_box(collider, node, box);
    }

    return collider;
}

Box* fw64_scene_get_initial_bounds(fw64Scene* scene) {
    return &scene->bounding_box;
}

fw64Allocator* fw64_scene_get_allocator(fw64Scene* scene) {
    return scene->allocator;
}

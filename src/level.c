#include "framework64/level.h"
#include "framework64/math.h"

#include <limits.h>
#include <string.h>

void fw64_level_chunk_info_init(fw64LevelChunkInfo* info) {
    memset(info, 0, sizeof(fw64LevelChunkInfo));
}

void fw64_level_init(fw64Level* level, fw64Engine* engine) {
    memset(level, 0, sizeof(fw64Level));
    level->engine = engine;
}

void fw64_level_uninit(fw64Level* level) {
    for (uint32_t i = 0; i < level->chunk_ref_count; i++) {
        fw64LevelChuckRef* ref = level->chunk_refs + i;

        if (ref->info.uninit_func) {
            ref->info.uninit_func(ref->handle, ref->info.scene_id, ref->scene, ref->info.callback_arg);
        }

        fw64_scene_delete(level->engine->assets, ref->scene, fw64_scene_get_allocator(ref->scene));
    }
}

void fw64_level_update(fw64Level* level) {
    for (uint32_t i = 0; i < level->chunk_ref_count; i++) {
        fw64LevelChuckRef* ref = level->chunk_refs + i;

        if (ref->info.update_func) {
            ref->info.uninit_func(ref->handle, ref->info.scene_id, ref->scene, ref->info.callback_arg);
        }
    }
}

static fw64LevelChuckRef* fw64_level_load_scene(fw64Level* level, fw64LevelChunkInfo* info) {
    if (level->chunk_ref_count >= FW64_LEVEL_CHUNK_COUNT)
        return NULL;

    fw64LevelChuckRef* ref = level->chunk_refs + level->chunk_ref_count;
    ref->scene = fw64_scene_load(level->engine->assets, info->scene_id, info->allocator);

    if (!ref->scene)
        return NULL;
    
    ref->info = *info;
    ref->handle = ++level->handle_counter;
    level->chunk_ref_count += 1;

    return ref;
}

uint32_t fw64_level_load_chunk(fw64Level* level, fw64LevelChunkInfo* info) {
    fw64LevelChuckRef* ref = fw64_level_load_scene(level, info);

    if (ref->info.init_func) {
        ref->info.init_func(ref->handle, ref->info.scene_id, ref->scene, ref->info.callback_arg);
    }

    if (!ref)
        return FW64_LEVEL_INVALID_CHUNK_HANDLE;
    else
        return ref->handle;
}

uint32_t fw64_level_load_chunk_at_pos(fw64Level* level, fw64LevelChunkInfo* info, Vec3* pos) {
    fw64LevelChuckRef* ref = fw64_level_load_scene(level, info);

    if (!ref)
        return FW64_LEVEL_INVALID_CHUNK_HANDLE;

    uint32_t node_count = fw64_scene_get_node_count(ref->scene);
    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* node = fw64_scene_get_node(ref->scene, i);

        vec3_add(&node->transform.position, &node->transform.position, pos);
        fw64_node_update(node);
    }

    fw64_scene_update_bounding(ref->scene);

    if (ref->info.init_func) {
        ref->info.init_func(ref->handle, ref->info.scene_id, ref->scene, ref->info.callback_arg);
    }

    return ref->handle;
}

int fw64_level_unload_chunk(fw64Level* level, uint32_t handle) {
    uint32_t target_index = UINT32_MAX;
    for (uint32_t i = 0; i < level->chunk_ref_count; i++) {
        if (level->chunk_refs[i].handle == handle) {
            target_index = i;
            break;
        }
    }

    if (target_index == UINT32_MAX)
        return 0;

    fw64LevelChuckRef* ref = level->chunk_refs + target_index;
    if (ref->info.uninit_func)
        ref->info.uninit_func(ref->handle, ref->info.scene_id, ref->scene, ref->info.callback_arg);
    
    fw64_scene_delete(level->engine->assets, ref->scene, fw64_scene_get_allocator(ref->scene));

    // move last item in chunk ref array to the deleted chunk's spot
    if (target_index != level->chunk_ref_count - 1) {
        fw64LevelChuckRef* swap_ref = level->chunk_refs + (level->chunk_ref_count - 1);
        memcpy(ref, swap_ref, sizeof(fw64LevelChuckRef));
    }

    level->chunk_ref_count -= 1;

    return 1;
}

int fw64_level_raycast(fw64Level* level, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit) {
    hit->distance = FLT_MAX;
    int result = 0;

    for (uint32_t i = 0; i < level->chunk_ref_count; i++) {
        fw64LevelChuckRef* ref = level->chunk_refs + i;
        fw64RaycastHit scene_hit;

        if (!fw64_collision_test_ray_box(origin, direction, fw64_scene_get_initial_bounds(ref->scene), &scene_hit.point, &scene_hit.distance))
            continue;

        int did_hit = fw64_scene_raycast(level->chunk_refs[i].scene, origin, direction, mask, &scene_hit);

        if (did_hit && scene_hit.distance < hit->distance) {
            *hit = scene_hit;
            result = 1;
        }
    }

    return result;
}

int fw64_level_moving_box_intersection(fw64Level* level, Box* box, Vec3* velocity, uint32_t mask, fw64IntersectMovingBoxQuery* result) {
    fw64_intersect_moving_box_query_init(result);
    int did_hit = 0;

    for (uint32_t i = 0; i < level->chunk_ref_count; i++) {
        fw64LevelChuckRef* ref = level->chunk_refs + i;
        Vec3 level_velocity = {0.0f, 0.0f, 0.0f};
        float first, last;

        if (!fw64_collision_test_moving_boxes(box, velocity, fw64_scene_get_initial_bounds(ref->scene), &level_velocity, &first, &last))
            continue;

        did_hit |= fw64_scene_moving_box_intersection(ref->scene, box, velocity, mask, result);

        if (result->count == Fw64_COLLISION_QUERY_RESULT_SIZE)
            break;
    }
    

    return did_hit;
}

int fw64_level_moving_sphere_intersection(fw64Level* level, Vec3* center, float radius, Vec3* velocity, uint32_t mask, fw64IntersectMovingSphereQuery* result) {
    fw64_intersect_moving_sphere_query_init(result);
    int did_hit = 0;

    for (uint32_t i = 0; i < level->chunk_ref_count; i++) {
        fw64LevelChuckRef* ref = level->chunk_refs + i;
        Vec3 out_point;
        float out_t;

        // rapid rejection test against bounding box of entire scene
        if(!fw64_collision_test_moving_sphere_box(center, radius, velocity, fw64_scene_get_initial_bounds(ref->scene), &out_point, &out_t))
            continue;

        did_hit |= fw64_scene_moving_sphere_intersection(ref->scene, center, radius, velocity, mask, result);

        if (result->count == Fw64_COLLISION_QUERY_RESULT_SIZE)
            break;
    }

    return did_hit;
}

int fw64_level_moving_spheres_dynamic_intersection(fw64Level* level, Vec3* center, float radius, Vec3* velocity, uint32_t mask, fw64IntersectMovingSphereQuery* result) {
    fw64_intersect_moving_sphere_query_init(result);
    int did_hit = 0;
    int node_count = fw64_level_get_dynamic_node_count(level);
    for (uint32_t i = 0; i < node_count; i++) {
        fw64Node* dynamic_node = fw64_level_get_dynamic_node(level, i);
        float out_t = FLT_MAX;

        // ignore nodes not in layer mask
        if (!(dynamic_node->layer_mask & mask))
            continue;
        
        Vec3 node_center;
        vec3_copy(&node_center, &dynamic_node->collider->transform->position);
        Vec3 node_extents;
        box_extents(&dynamic_node->collider->bounding, &node_extents);
        float node_radius = fw64_minf(node_extents.x, node_extents.z); // todo: implement sphere colliders
        Vec3 node_vel = {0.0f, 0.0f, 0.0f}; // todo: possibly store this instead of assuming 0
        IntersectMovingSphereResult* sphere_intersect = &result->results[result->count];
        if(fw64_collision_test_moving_spheres(center, radius, velocity, &node_center, node_radius, &node_vel, &out_t)) {
            did_hit |= 1;
            sphere_intersect->node = dynamic_node;
            sphere_intersect->distance = out_t;
            Vec3 vel_norm;
            vec3_copy(&vel_norm, velocity);
            vec3_normalize(&vel_norm);
            vec3_add_and_scale(&sphere_intersect->point, center, &vel_norm, out_t);
            ++result->count;
        }

        if (result->count == Fw64_COLLISION_QUERY_RESULT_SIZE)
            break;
    }

    return did_hit;
}


void fw64_level_draw_camera(fw64Level* level, fw64Camera* camera) {
    fw64Renderer* renderer = level->engine->renderer;
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(camera, &frustum);

    for (uint32_t i = 0; i < level->chunk_ref_count; i++) {
        fw64LevelChuckRef* ref = level->chunk_refs + i;

        if (!fw64_frustum_intersects_box(&frustum, fw64_scene_get_initial_bounds(ref->scene)))
            continue;

        if (ref->info.draw_func) {
            ref->info.draw_func(ref->handle, ref->info.scene_id, ref->scene, ref->info.callback_arg);
        }

        fw64_scene_draw_frustrum(ref->scene, renderer, &frustum);
    }
}

void fw64_level_draw_camera_all(fw64Level* level, fw64Camera* camera) {
    fw64Renderer* renderer = level->engine->renderer;
    fw64Frustum frustum;
    fw64_camera_extract_frustum_planes(camera, &frustum);

    for (uint32_t i = 0; i < level->chunk_ref_count; i++) {
        fw64LevelChuckRef* ref = level->chunk_refs + i;

        if (!fw64_frustum_intersects_box(&frustum, fw64_scene_get_initial_bounds(ref->scene)))
            continue;

        if (ref->info.draw_func) {
            ref->info.draw_func(ref->handle, ref->info.scene_id, ref->scene, ref->info.callback_arg);
        }

        fw64_scene_draw_all(ref->scene, renderer);
    }
}

void fw64_level_add_dynamic_node(fw64Level* level, fw64Node* node) {
    if (level->dynamic_node_count == FW64_LEVEL_DYNAMIC_NODE_COUNT)
        return;

    level->dynamic_nodes[level->dynamic_node_count++] = node;
}

void fw64_level_remove_dynamic_node(fw64Level* level, fw64Node* node) {
    for (int i = 0; i < level->dynamic_node_count; i++) {
        if (level->dynamic_nodes[i] == node) {
            level->dynamic_node_count -= 1;
            level->dynamic_nodes[i] = level->dynamic_nodes[level->dynamic_node_count];
            
            return;
        }
    }
}

uint32_t fw64_level_get_dynamic_node_count(fw64Level* level) {
    return level->dynamic_node_count;
}

fw64Node* fw64_level_get_dynamic_node(fw64Level* level, uint32_t index) {
    return level->dynamic_nodes[index];
}

uint32_t fw64_level_get_chunk_count(fw64Level* level) {
    return level->chunk_ref_count;
}

fw64Scene* fw64_level_get_chunk_by_index(fw64Level* level, uint32_t index) {
    return level->chunk_refs[index].scene;
}

fw64Scene* fw64_level_get_chunk_by_handle(fw64Level* level, uint32_t handle) {
    for (uint32_t i = 0; i < level->chunk_ref_count; i++) {
        fw64LevelChuckRef* ref = level->chunk_refs + i;

        if (ref->handle == handle)
            return ref->scene;
    }

    return NULL;
}

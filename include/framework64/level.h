#pragma once

/** \file level.h */

#include "framework64/engine.h"
#include "framework64/scene.h"
#include "framework64/util/bump_allocator.h"

#include <limits.h>
#include <stdint.h>

#ifndef FW64_LEVEL_CHUNK_COUNT
#define FW64_LEVEL_CHUNK_COUNT 9
#endif

#ifndef FW64_LEVEL_DYNAMIC_NODE_COUNT
#define FW64_LEVEL_DYNAMIC_NODE_COUNT 32
#endif

#define FW64_LEVEL_INVALID_CHUNK_HANDLE UINT32_MAX

/**
 * Callback function used for level events.
 * @param chunk_id the chunk id returned from the call to \ref fw64_level_load_chunk
 * @param scene_id the asset id of the scene
 * @param scene the scene object
 * @param arg user supplied callback
 */
typedef void(*LevelChunkCallback)(uint32_t chunk_id, int scene_id, fw64Scene* scene, void* arg);

/** Describes a chunk that will be loaded into the level */
typedef struct {
    /** The id of the scene asset that will be loaded */
    int scene_id;

    /** The allocator object that will be used to load the scene */
    fw64Allocator* allocator;

    /** Function that will be called if the chunk is successfully loaded via \ref fw64_level_load_chunk*/
    LevelChunkCallback init_func;

    /** Function that will be called from the level's update method. */
    LevelChunkCallback update_func;

    /** Function that will be called if the chunk will be rendered. */
    LevelChunkCallback draw_func;

    /** Function that will be called when a chunk is removed via \ref fw64_level_unload_chunk */
    LevelChunkCallback uninit_func;

    /** User supplied callback arg that will be passed to all functions. */
    void* callback_arg;
} fw64LevelChunkInfo;

void fw64_level_chunk_info_init(fw64LevelChunkInfo* info);

typedef struct {
    fw64LevelChunkInfo info;
    fw64Scene* scene;
    uint32_t handle;
} fw64LevelChuckRef;

typedef struct {
    fw64Engine* engine;
    fw64Allocator* allocator;
    fw64LevelChuckRef chunk_refs[FW64_LEVEL_CHUNK_COUNT];
    uint32_t chunk_ref_count;
    fw64Node* dynamic_nodes[FW64_LEVEL_DYNAMIC_NODE_COUNT];
    uint32_t dynamic_node_count;
    uint32_t handle_counter;
} fw64Level;

void fw64_level_init(fw64Level* level, fw64Engine* engine);
void fw64_level_uninit(fw64Level* level);

/** 
 * Loads a scene as a level chunk which is described by the supplied info struct.
 * If the scene is sucessfully loaded the associated init method will be called
 * @returns a handle identifier to this chunk or FW64_LEVEL_INVALID_CHUNK_HANDLE if loading failed
 */
uint32_t fw64_level_load_chunk(fw64Level* level, fw64LevelChunkInfo* info);

/**
 * Loads a scene level chunk as in \ref fw64_level_load_chunk and additionally applies a translation offset to each node in the scene
 * @returns a handle identifier to this chunk or FW64_LEVEL_INVALID_CHUNK_HANDLE if loading failed
 */
uint32_t fw64_level_load_chunk_at_pos(fw64Level* level, fw64LevelChunkInfo* info, Vec3* pos);

/**
 * Unloads a level chuck with the supplied handle and calls it's associated uninit method if one was specified.
 * This method will also delete the scene using the specified allocator (or default if none specified)
 * @returns non zero value if a chunk with the supplied handle was removed
 */
int fw64_level_unload_chunk(fw64Level* level, uint32_t handle);

uint32_t fw64_level_get_chunk_count(fw64Level* level);
fw64Scene* fw64_level_get_chunk_by_index(fw64Level* level, uint32_t index);
fw64Scene* fw64_level_get_chunk_by_handle(fw64Level* level, uint32_t handle);

/** Will call each chunk's associated update method. */
void fw64_level_update(fw64Level* level);

/**
 * Tests each chunk's bounding box against the supplied camera's view frustum.
 * If the test passes will call the assoicated draw function.
 */
void fw64_level_draw_camera(fw64Level* level, fw64Camera* camera);

int fw64_level_raycast(fw64Level* level, Vec3* origin, Vec3* direction, uint32_t mask, fw64RaycastHit* hit);
int fw64_level_moving_box_intersection(fw64Level* level, Box* box, Vec3* velocity, uint32_t mask, fw64IntersectMovingBoxQuery* result);

void fw64_level_add_dyanmic_node(fw64Level* level, fw64Node* node);
void fw64_level_remove_dynamic_node(fw64Level* level, fw64Node* node);
uint32_t fw64_level_get_dynamic_node_count(fw64Level* level);
fw64Node* fw64_level_get_dynamic_node(fw64Level* level, uint32_t index);
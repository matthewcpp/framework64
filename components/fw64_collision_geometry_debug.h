#pragma once

#include <framework64/asset_database.h>
#include <framework64/engine.h>
#include <framework64/scene.h>
#include <framework64/util/bump_allocator.h>

#define FW64_COLLISION_GEOMETRY_DEBUG_MAX_GRID_CELL_COUNT (16 * 16)

typedef enum {
    FW64_COLLISION_GEOMETRY_DEBUG_LAYER_CELL_BOUNDINGS = 1 << 25,
    FW64_COLLISION_GEOMETRY_DEBUG_LAYER_BOUNDING_VOLUMES = 1 << 26,
    FW64_COLLISION_GEOMETRY_DEBUG_LAYER_FLOOR_TRIANGLES = 1 << 27,
    FW64_COLLISION_GEOMETRY_DEBUG_LAYER_WALL_TRIANGLES = 1 << 28,
    FW64_COLLISION_GEOMETRY_DEBUG_LAYER_CEILING_TRIANGLES = 1 << 29,

    FW64_COLLISION_GEOMETRY_DEBUG_LAYER_MASK_TRIANGLES = 
        FW64_COLLISION_GEOMETRY_DEBUG_LAYER_FLOOR_TRIANGLES |
        FW64_COLLISION_GEOMETRY_DEBUG_LAYER_WALL_TRIANGLES |
        FW64_COLLISION_GEOMETRY_DEBUG_LAYER_CEILING_TRIANGLES,

    FW64_COLLISION_GEOMETRY_DEBUG_LAYER_MASK_ALL =
        FW64_COLLISION_GEOMETRY_DEBUG_LAYER_CELL_BOUNDINGS |
        FW64_COLLISION_GEOMETRY_DEBUG_LAYER_BOUNDING_VOLUMES |
        FW64_COLLISION_GEOMETRY_DEBUG_LAYER_MASK_TRIANGLES

} CollisionGeometryDebugLayerMask;

typedef struct {
    fw64BumpAllocator bump_allocator;
    fw64Engine* engine;
    fw64Scene* scene;
    fw64DataSource* data_source;
    fw64RenderPass* renderpass;

    const fw64Transform* target;
    const fw64CollisionGeometry* collision_geometry;
    const fw64Camera* camera;

    /** Holds the offsets for each cell's debug scene in the file */
    uint32_t cell_scene_toc[FW64_COLLISION_GEOMETRY_DEBUG_MAX_GRID_CELL_COUNT];

    /** The number of cells in the toc */
    uint32_t cell_scene_count;

    uint32_t previous_cell_index;
    fw64LayerMask layer_mask;
} fw64CollisionGeometryDebug;

#ifdef __cplusplus
extern "C" {
#endif

/** Note: currently this will allocate from the default allocator */
void fw64_collision_geometry_debug_init(fw64CollisionGeometryDebug* geometry_debug, fw64Engine* engine, size_t bump_size);
void fw64_collision_geometry_debug_uninit(fw64CollisionGeometryDebug* geometry_debug);

int fw64_collision_geometry_debug_load(fw64CollisionGeometryDebug* geometry_debug, fw64AssetId asset_id);
void fw64_collision_geometry_debug_track(fw64CollisionGeometryDebug* geometry_debug, const fw64CollisionGeometry* collision_geometry, const fw64Transform* target, const fw64Camera* camera);
void fw64_collision_geometry_debug_update(fw64CollisionGeometryDebug* geometry_debug);
void fw64_collision_geometry_debug_draw(fw64CollisionGeometryDebug* geometry_debug);

#ifdef __cplusplus
}
#endif

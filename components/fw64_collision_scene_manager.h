#pragma once

#include "fw64_debug_character.h"

#include <framework64/engine.h>
#include <framework64/scene.h>

typedef enum {
    /** Displays only the original Scene */
    FW64_COLLISION_SCENE_MANAGER_DISPLAY_MODE_DEFAULT,
    FW64_COLLISION_SCENE_MANAGER_DISPLAY_MODE_ACTIVE_CELL_WIREFRAME_OVERLAY
} fw64CollisionSceneManagerDisplayMode;

/** This needs to be kept in sync with CollisionDebugLayers in CollisionGeometryDebug.js */
typedef enum {
    FW64_COLLISION_SCENE_DEBUG_LAYER_WIRE_TRIANGLES = 1,
    FW64_COLLISION_SCENE_DEBUG_LAYER_GRID = 2
} fw64CollisionSceneManagerLayers;

typedef struct {
    fw64Engine* engine;
    fw64Allocator* allocator;
    fw64CollisionSceneManagerDisplayMode display_mode;
    fw64LayerMask static_gemoetry_layer_mask;

    fw64Scene* scene;
    fw64RenderPass* static_scene_renderpass;

    /** When drawing the overlay grid, the character's node position will be used to determine which grid to draw */
    fw64Character* character;
    fw64DebugCharacter debug_character;

    fw64Scene* collision_wireframe;
    fw64RenderPass* wireframe_renderpass;
    fw64Frustum view_frustum;

    int show_grid;
} fw64CollisionSceneManager;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_collision_scene_manager_init(fw64CollisionSceneManager* manager, fw64Engine* engine, fw64Display* display, fw64Allocator* allocator);
void fw64_collision_scene_manager_uninit(fw64CollisionSceneManager* manager);
void fw64_collision_scene_manager_update(fw64CollisionSceneManager* manager);

fw64Scene* fw64_collision_scene_manager_load_scene(fw64CollisionSceneManager* manager, fw64AssetId scene_id, fw64AssetId wire_scene_id, fw64LayerMask static_gemoetry_layer_mask);

/** TEMP: this should be removed when the manager is refactored */
void fw64_collision_scene_manager_set_character(fw64CollisionSceneManager* manager, fw64Character* character);

/** Sets the camera for the renderpasses.
 *  Note, when this function is called the camera's view frustum will be calculated and used for subsequent draw calls.
 *  Ensure that it has been fully updated before this method is called.
 */
void fw64_collision_scene_manager_set_camera(fw64CollisionSceneManager* manager, fw64Camera* camera);

void fw64_collision_scene_manager_draw_scene(fw64CollisionSceneManager* manager, fw64LayerMask layer_mask);

#ifdef __cplusplus
}
#endif

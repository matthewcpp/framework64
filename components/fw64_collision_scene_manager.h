#pragma once

#include <framework64/engine.h>

typedef enum {
    FW64_COLLISION_SCENE_MANAGER_DISPLAY_MODE_DEFAULT,
    FW64_COLLISION_SCENE_MANAGER_DISPLAY_MODE_WIREFRAME_OVERLAY,
    FW64_COLLISION_SCENE_MANAGER_DISPLAY_MODE_WIREFRAME_ONLY
} fw64CollisionSceneManagerDisplayMode;

typedef struct {
    fw64Engine* engine;
    fw64Allocator* allocator;
    fw64CollisionSceneManagerDisplayMode display_mode;
    uint32_t static_gemoetry_layer_id;

    fw64Scene* scene;
    fw64RenderPass* static_scene_renderpass;
    fw64RenderPass* scene_renderpass;

    fw64Scene* collision_wireframe;
    fw64RenderPass* wireframe_renderpass;
    fw64Frustum view_frustum;
} fw64CollisionSceneManager;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_collision_scene_manager_init(fw64CollisionSceneManager* manager, fw64Engine* engine, fw64Display* display, fw64Allocator* allocator);
void fw64_collision_scene_manager_uninit(fw64CollisionSceneManager* manager);

fw64Scene* fw64_collision_scene_manager_load_scene(fw64CollisionSceneManager* manager, fw64AssetId scene_id, fw64AssetId wire_scene_id, uint32_t static_gemoetry_layer_id);

/** Sets the camera for the renderpasses.
 *  Note, when this function is called the camera's view frutum will be calculated and used for subsequent draw calls.
 *  Ensure that it has been fully updated before this method is called.
 */
void fw64_collision_scene_manager_set_camera(fw64CollisionSceneManager* manager, fw64Camera* camera);

void fw64_collision_scene_manager_draw_scene(fw64CollisionSceneManager* manager);
#ifdef __cplusplus
}
#endif

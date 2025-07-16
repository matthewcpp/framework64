#include "fw64_collision_scene_manager.h"

#include <framework64/scene.h>

void fw64_collision_scene_manager_init(fw64CollisionSceneManager* manager, fw64Engine* engine, fw64Display* display, fw64Allocator* allocator) {
    manager->engine = engine;
    manager->allocator = allocator;
    manager->static_gemoetry_layer_id = 1;
    manager->show_grid = 1;
    manager->display_mode = FW64_COLLISION_SCENE_MANAGER_DISPLAY_MODE_DEFAULT;

    manager->scene = NULL;
    manager->scene_renderpass = fw64_renderpass_create(display, allocator);
    manager->static_scene_renderpass = fw64_renderpass_create(display, allocator);

    manager->collision_wireframe = NULL;
    manager->wireframe_renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_primitive_mode(manager->wireframe_renderpass, FW64_PRIMITIVE_MODE_LINES);
}

void fw64_collision_scene_manager_uninit(fw64CollisionSceneManager* manager) {
    fw64_renderpass_delete(manager->scene_renderpass);
    fw64_renderpass_delete(manager->static_scene_renderpass);
    fw64_renderpass_delete(manager->wireframe_renderpass);

    if (manager->scene) {
        fw64_scene_delete(manager->scene);
    }

    if (manager->collision_wireframe) {
        fw64_scene_delete(manager->collision_wireframe);
    }
}

fw64Scene* fw64_collision_scene_manager_load_scene(fw64CollisionSceneManager* manager, fw64AssetId scene_id, fw64AssetId wire_scene_id, uint32_t static_gemoetry_layer_id) {
    manager->scene = fw64_assets_load_scene(manager->engine->assets, scene_id, manager->allocator);
    manager->collision_wireframe = fw64_assets_load_scene(manager->engine->assets, wire_scene_id, manager->allocator);
    manager->static_gemoetry_layer_id = static_gemoetry_layer_id;

    return manager->scene;
}

void fw64_collision_scene_manager_set_camera(fw64CollisionSceneManager* manager, fw64Camera* camera) {
    fw64_renderpass_set_camera(manager->scene_renderpass, camera);
    fw64_renderpass_set_camera(manager->static_scene_renderpass, camera);
    fw64_renderpass_set_camera(manager->wireframe_renderpass, camera);

    fw64_camera_extract_frustum_planes(camera, &manager->view_frustum);
}

void fw64_collision_scene_manager_draw_scene(fw64CollisionSceneManager* manager){
    fw64_renderpass_begin(manager->static_scene_renderpass);
    fw64_scene_draw_frustrum(manager->scene, manager->static_scene_renderpass, &manager->view_frustum, manager->static_gemoetry_layer_id);
    fw64_renderpass_end(manager->static_scene_renderpass);

    fw64_renderpass_begin(manager->scene_renderpass);
    fw64_scene_draw_frustrum(manager->scene, manager->scene_renderpass, &manager->view_frustum, ~manager->static_gemoetry_layer_id);
    fw64_renderpass_end(manager->scene_renderpass);

    uint32_t wire_layer_mask = FW64_COLLISION_SCENE_DEBUG_LAYER_WIRE_TRIANGLES;
    if (manager->show_grid) {
        wire_layer_mask |= FW64_COLLISION_SCENE_DEBUG_LAYER_GRID;
    }

    fw64_renderpass_begin(manager->wireframe_renderpass);
    fw64_scene_draw_frustrum(manager->collision_wireframe, manager->wireframe_renderpass, &manager->view_frustum, wire_layer_mask);
    fw64_renderpass_end(manager->wireframe_renderpass);

    fw64_renderer_submit_renderpass(manager->engine->renderer, manager->static_scene_renderpass);
    fw64_renderer_submit_renderpass(manager->engine->renderer, manager->wireframe_renderpass);
    fw64_renderer_submit_renderpass(manager->engine->renderer, manager->scene_renderpass);
}

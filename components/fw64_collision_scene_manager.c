#include "fw64_collision_scene_manager.h"

#include <framework64/scene.h>

void fw64_collision_scene_manager_init(fw64CollisionSceneManager* manager, fw64Engine* engine, fw64Display* display, fw64Allocator* allocator) {
    manager->engine = engine;
    manager->character = NULL;
    manager->allocator = allocator;
    manager->static_gemoetry_layer_mask = FW64_LAYER_MASK_ALL_LAYERS;
    manager->show_grid = 1;
    manager->display_mode = FW64_COLLISION_SCENE_MANAGER_DISPLAY_MODE_ACTIVE_CELL_WIREFRAME_OVERLAY;

    manager->scene = NULL;
    manager->static_scene_renderpass = fw64_renderpass_create(display, allocator);

    manager->collision_wireframe = NULL;
    manager->wireframe_renderpass = fw64_renderpass_create(display, allocator);
    fw64_renderpass_set_primitive_mode(manager->wireframe_renderpass, FW64_PRIMITIVE_MODE_LINES);
}

void fw64_collision_scene_manager_uninit(fw64CollisionSceneManager* manager) {
    fw64_renderpass_delete(manager->static_scene_renderpass);
    fw64_renderpass_delete(manager->wireframe_renderpass);

    if (manager->scene) {
        fw64_scene_delete(manager->scene);
    }

    if (manager->collision_wireframe) {
        fw64_scene_delete(manager->collision_wireframe);
    }
}

void fw64_collision_scene_manager_update(fw64CollisionSceneManager* manager) {
    fw64_debug_character_update(&manager->debug_character);
}

fw64Scene* fw64_collision_scene_manager_load_scene(fw64CollisionSceneManager* manager, fw64AssetId scene_id, fw64AssetId wire_scene_id, fw64LayerMask static_gemoetry_layer_mask) {
    manager->scene = fw64_assets_load_scene(manager->engine->assets, scene_id, manager->allocator);

    if (wire_scene_id != FW64_INVALID_ASSET_ID) {
        manager->collision_wireframe = fw64_assets_load_scene(manager->engine->assets, wire_scene_id, manager->allocator);
    }

    manager->static_gemoetry_layer_mask = static_gemoetry_layer_mask;

    return manager->scene;
}

void fw64_collision_scene_manager_set_character(fw64CollisionSceneManager* manager, fw64Character* character) {
    manager->character = character;
    fw64_debug_character_init(&manager->debug_character, character);
}

void fw64_collision_scene_manager_set_camera(fw64CollisionSceneManager* manager, fw64Camera* camera) {
    fw64_renderpass_set_camera(manager->static_scene_renderpass, camera);
    fw64_renderpass_set_camera(manager->wireframe_renderpass, camera);

    fw64_camera_extract_frustum_planes(camera, &manager->view_frustum);
}

static void fw64_collision_scene_manager_draw_default_scene(fw64CollisionSceneManager* manager, fw64LayerMask layer_mask) {
    fw64_renderpass_begin(manager->static_scene_renderpass);
    fw64_scene_draw_frustrum(manager->scene, manager->static_scene_renderpass, &manager->view_frustum, layer_mask);
    fw64_renderpass_end(manager->static_scene_renderpass);

    fw64_renderer_submit_renderpass(manager->engine->renderer, manager->static_scene_renderpass);
}

#define DEBUG_SCENE_COLLISION_GEOMETRY_GEOMETRY_NODES_START 2

static void fw64_collision_scene_manager_draw_mode_active_cell_wireframe(fw64CollisionSceneManager* manager) {
    const fw64CollisionGeometry* geometry = manager->scene->collision_geometry;
    IVec2 grid_pos;

    if (manager->character && fw64_collision_geometry_get_cell_coordinates_vec3(geometry, &manager->character->node->transform.position, &grid_pos)) {
        uint32_t cell_index = fw64_collision_geometry_get_cell_index(geometry, grid_pos.x, grid_pos.y);
        fw64Node* current_geometry_node = fw64_scene_get_node(manager->collision_wireframe, DEBUG_SCENE_COLLISION_GEOMETRY_GEOMETRY_NODES_START + cell_index);
        fw64_renderpass_draw_static_mesh(manager->wireframe_renderpass, current_geometry_node->mesh_instance);

        if (manager->show_grid) {
            uint32_t grid_index_start = DEBUG_SCENE_COLLISION_GEOMETRY_GEOMETRY_NODES_START + fw64_collision_geometry_get_cell_count(geometry) + 1;
            fw64Node* current_grid_node = fw64_scene_get_node(manager->collision_wireframe, grid_index_start + cell_index);
            fw64_renderpass_draw_static_mesh(manager->wireframe_renderpass, current_grid_node->mesh_instance);
        }
    }
}

void fw64_collision_scene_manager_draw_scene(fw64CollisionSceneManager* manager, fw64LayerMask layer_mask){
    if (!manager->collision_wireframe) {
        fw64_collision_scene_manager_draw_default_scene(manager, layer_mask);
        return;
    }

    switch(manager->display_mode) {
        case FW64_COLLISION_SCENE_MANAGER_DISPLAY_MODE_DEFAULT:
            fw64_collision_scene_manager_draw_default_scene(manager, layer_mask);
            break;

        case FW64_COLLISION_SCENE_MANAGER_DISPLAY_MODE_ACTIVE_CELL_WIREFRAME_OVERLAY:
            fw64_collision_scene_manager_draw_default_scene(manager, layer_mask);
            fw64_renderpass_begin(manager->wireframe_renderpass);
            fw64_collision_scene_manager_draw_mode_active_cell_wireframe(manager);
            fw64_debug_character_draw(&manager->debug_character, manager->wireframe_renderpass);
            fw64_renderpass_end(manager->wireframe_renderpass);

            fw64_renderer_submit_renderpass(manager->engine->renderer, manager->wireframe_renderpass);
            break;
    }
}

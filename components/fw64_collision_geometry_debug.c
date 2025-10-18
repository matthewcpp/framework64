#include "fw64_collision_geometry_debug.h"

#include <limits.h>

#define FW64_COLLISION_GEOMETRY_DEBUG_INVALID_CELL_INDEX UINT32_MAX

void fw64_collision_geometry_debug_init(fw64CollisionGeometryDebug* geometry_debug, fw64Engine* engine, size_t bump_size){
    geometry_debug->engine = engine;
    geometry_debug->scene = NULL;
    geometry_debug->data_source = NULL;
    geometry_debug->previous_cell_index = FW64_COLLISION_GEOMETRY_DEBUG_INVALID_CELL_INDEX;
    geometry_debug->layer_mask = FW64_COLLISION_GEOMETRY_DEBUG_LAYER_CELL_BOUNDINGS | FW64_COLLISION_GEOMETRY_DEBUG_LAYER_MASK_TRIANGLES;
    fw64_bump_allocator_init(&geometry_debug->bump_allocator, bump_size);
    fw64_collision_geometry_debug_track(geometry_debug, NULL, NULL, NULL);

    geometry_debug->renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), fw64_default_allocator());
    fw64_renderpass_set_primitive_mode(geometry_debug->renderpass, FW64_PRIMITIVE_MODE_LINES);
    //fw64_renderpass_set_clear_flags(geometry_debug->renderpass, FW64_CLEAR_FLAG_DEPTH);
}

static void fw64_collision_geometry_debug_unload(fw64CollisionGeometryDebug* geometry_debug) {
    if (geometry_debug->data_source) {
        fw64_assets_close_datasource(geometry_debug->engine->assets, geometry_debug->data_source);
        geometry_debug->data_source = NULL;
    }

    if (geometry_debug->scene) {
        fw64_scene_delete(geometry_debug->scene);
        geometry_debug->scene = NULL;
    }
}

void fw64_collision_geometry_debug_uninit(fw64CollisionGeometryDebug* geometry_debug) {
    fw64_collision_geometry_debug_unload(geometry_debug);
    fw64_bump_allocator_uninit(&geometry_debug->bump_allocator);
}

int fw64_collision_geometry_debug_load(fw64CollisionGeometryDebug* geometry_debug, fw64AssetId asset_id) {
    fw64_collision_geometry_debug_unload(geometry_debug);
    fw64_bump_allocator_reset(&geometry_debug->bump_allocator);

    geometry_debug->data_source = fw64_assets_open_datasource(geometry_debug->engine->assets, asset_id);

    if (!geometry_debug) {
        return 0;
    }

    fw64_data_source_read(geometry_debug->data_source, &geometry_debug->cell_scene_count, sizeof(uint32_t), 1);

    if (geometry_debug->cell_scene_count > FW64_COLLISION_GEOMETRY_DEBUG_MAX_GRID_CELL_COUNT) {
        fw64_assets_close_datasource(geometry_debug->engine->assets, geometry_debug->data_source);
        geometry_debug->data_source = NULL;
        return 0;
    }

    fw64_data_source_read(geometry_debug->data_source, geometry_debug->cell_scene_toc, sizeof(uint32_t), geometry_debug->cell_scene_count);

    return 1;
}

void fw64_collision_geometry_debug_track(fw64CollisionGeometryDebug* geometry_debug, const fw64CollisionGeometry* collision_geometry, const fw64Transform* target, const fw64Camera* camera) {
    geometry_debug->collision_geometry = collision_geometry;
    geometry_debug->target = target;
    geometry_debug->camera = camera;
}

void fw64_collision_geometry_debug_update(fw64CollisionGeometryDebug* geometry_debug) {
    IVec3 cell_coords;
    fw64_collision_geometry_get_cell_coordinates_vec3(geometry_debug->collision_geometry, &geometry_debug->target->position, &cell_coords);
    const uint32_t active_cell_index = fw64_collision_geometry_get_cell_index(geometry_debug->collision_geometry, cell_coords.x, cell_coords.y, cell_coords.z);

    if (!geometry_debug->data_source || active_cell_index == geometry_debug->previous_cell_index) {
        return;
    }

    if (geometry_debug->scene) {
        fw64_scene_delete(geometry_debug->scene);
    }
    
    const size_t cell_scene_offset = (size_t)geometry_debug->cell_scene_toc[active_cell_index];
    fw64_data_source_seek(geometry_debug->data_source, cell_scene_offset);

    fw64_bump_allocator_reset(&geometry_debug->bump_allocator);
    geometry_debug->scene = fw64_scene_load_from_datasource(geometry_debug->data_source, geometry_debug->engine->assets, &geometry_debug->bump_allocator.interface);

    geometry_debug->previous_cell_index = active_cell_index;

    fw64_renderpass_begin(geometry_debug->renderpass);
    fw64_scene_draw_all(geometry_debug->scene, geometry_debug->renderpass, geometry_debug->layer_mask);
    fw64_renderpass_end(geometry_debug->renderpass);
}

void fw64_collision_geometry_debug_draw(fw64CollisionGeometryDebug* geometry_debug) {
    if (!geometry_debug->scene) {
        return;
    }

    fw64_renderpass_set_camera(geometry_debug->renderpass, geometry_debug->camera);
    fw64_renderer_submit_renderpass(geometry_debug->engine->renderer, geometry_debug->renderpass);
}

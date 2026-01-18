#include "fw64_character_debug.h"

void fw64_character_debug_init(fw64CharacterDebug* debug, fw64Engine* engine, size_t collision_geometry_bump_size, const fw64DebugPrimitivesConfig* config, fw64Allocator* allocator){
    debug->engine = engine;
    debug->renderpass = fw64_renderpass_create(fw64_displays_get_primary(engine->displays), fw64_default_allocator());
    fw64_renderpass_set_primitive_mode(debug->renderpass, FW64_PRIMITIVE_MODE_LINES);
    //fw64_renderpass_set_clear_flags(geometry_debug->renderpass, FW64_CLEAR_FLAG_DEPTH);

    // todo make a default flag, also make this configurable via UI
    debug->collision_geometry.layer_mask = FW64_COLLISION_GEOMETRY_DEBUG_LAYER_CELL_BOUNDINGS;

    fw64_collision_geometry_debug_init(&debug->collision_geometry, engine, collision_geometry_bump_size, allocator);
    fw64_debug_primitives_init(&debug->primitives, engine, config, allocator);

    debug->character = NULL;
    debug->capsule_handle = FW64_DEBUG_PRIMITIVES_INVALID_HANDLE;
}

void fw64_character_debug_uninit(fw64CharacterDebug* debug) {
    fw64_collision_geometry_debug_uninit(&debug->collision_geometry);
}

void fw64_character_debug_track(fw64CharacterDebug* debug, fw64Character* character, fw64AssetId debug_geometry_scene, fw64Camera* camera) {
    fw64_collision_geometry_debug_load(&debug->collision_geometry, debug_geometry_scene);
    fw64_collision_geometry_debug_track(&debug->collision_geometry, character->scene->collision_geometry, &character->node->transform, camera);

    debug->character = character;
    if (debug->capsule_handle == FW64_DEBUG_PRIMITIVES_INVALID_HANDLE) {
        debug->capsule_handle = fw64_debug_primitives_add_capsule(&debug->primitives, &character->capsule);
    } else {
        fw64_debug_primitives_update_capsule(&debug->primitives, debug->capsule_handle, &debug->character->capsule);
    }
}

void fw64_character_debug_update(fw64CharacterDebug* debug) {
    fw64_collision_geometry_debug_update(&debug->collision_geometry);
}

void fw64_character_debug_fixed_update(fw64CharacterDebug* debug) {
    if (debug->capsule_handle != FW64_DEBUG_PRIMITIVES_INVALID_HANDLE) {
        fw64_debug_primitives_update_capsule(&debug->primitives, debug->capsule_handle, &debug->character->capsule);
    }
}

void fw64_character_debug_draw(fw64CharacterDebug* debug) {
    fw64_renderpass_set_camera(debug->renderpass, debug->collision_geometry.camera);

    fw64_renderpass_begin(debug->renderpass);
    fw64_collision_geometry_debug_draw(&debug->collision_geometry, debug->renderpass);
    fw64_debug_primitives_draw(&debug->primitives, debug->renderpass);
    fw64_renderpass_end(debug->renderpass);

    fw64_renderer_submit_renderpass(debug->engine->renderer, debug->renderpass);
}

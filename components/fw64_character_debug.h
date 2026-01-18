#pragma once

#include "fw64_character.h"
#include "fw64_collision_geometry_debug.h"
#include "fw64_debug_primitives.h"

typedef struct {
    fw64CollisionGeometryDebug collision_geometry;
    fw64DebugPrimitives primitives;
    fw64Character* character;
    fw64DebugPrimitiveHandle capsule_handle;
    fw64Engine* engine;
    fw64RenderPass* renderpass;
} fw64CharacterDebug;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_character_debug_init(fw64CharacterDebug* debug, fw64Engine* engine, size_t collision_geometry_bump_size, const fw64DebugPrimitivesConfig* config, fw64Allocator* allocator);
void fw64_character_debug_uninit(fw64CharacterDebug* debug);

/** Sets up the component to track a character in a given scene via a supplied camera */
void fw64_character_debug_track(fw64CharacterDebug* debug, fw64Character* character, fw64AssetId debug_geometry_scene, fw64Camera* camera);

void fw64_character_debug_update(fw64CharacterDebug* debug);
void fw64_character_debug_fixed_update(fw64CharacterDebug* debug);
void fw64_character_debug_draw(fw64CharacterDebug* debug);

#ifdef __cplusplus
}
#endif

#pragma once

#include "fw64_character.h"

#include <framework64/mesh_instance.h>

typedef struct {
    fw64Character* character;
    fw64Mesh* ledge_grab_mesh;
    fw64MeshInstance* ledge_grab_mesh_instance;
} fw64DebugCharacter;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_debug_character_init(fw64DebugCharacter* debug, fw64Character* character);
void fw64_debug_character_update(fw64DebugCharacter* debug);
void fw64_debug_character_draw(fw64DebugCharacter* debug, fw64RenderPass* renderpass);

#ifdef __cplusplus
}
#endif

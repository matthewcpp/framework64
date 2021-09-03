#ifndef FW64_MESH_H
#define FW64_MESH_H

#include "framework64/box.h"
#include "framework64/texture.h"

#include <stdint.h>

typedef enum {
    FW64_SHADING_MODE_UNSET,
    FW64_SHADING_MODE_UNLIT_VERTEX_COLORS,
    FW64_SHADING_MODE_UNLIT_TEXTURED,
    FW64_SHADING_MODE_GOURAUD,
    FW64_SHADING_MODE_GOURAUD_TEXTURED,
    FW64_SHADING_MODE_SPRITE,
    FW64_SHADING_MODE_FILL
} fw64ShadingMode;


typedef struct fw64Mesh fw64Mesh;
#ifdef __cplusplus
extern "C" {
#endif

/**
Initializes a mesh for manual construction.
*/
void fw64_mesh_init(fw64Mesh* mesh);

/**
Cleans up a mesh that was manually constructed after calling \ref mesh_init
*/
void fw64_mesh_uninit(fw64Mesh* mesh);

void fw64_mesh_get_bounding_box(fw64Mesh* mesh, Box* box);

#ifdef __cplusplus
}
#endif

#endif

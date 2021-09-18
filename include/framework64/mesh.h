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

typedef struct fw64Material fw64Material;
typedef struct fw64Primitive fw64Primitive;
typedef struct fw64Mesh fw64Mesh;

#ifdef __cplusplus
extern "C" {
#endif

/**
Cleans up a mesh that was manually constructed after calling \ref mesh_init
*/
void fw64_mesh_uninit(fw64Mesh* mesh);

void fw64_mesh_get_bounding_box(fw64Mesh* mesh, Box* box);
int fw64_mesh_get_primitive_count(fw64Mesh* mesh);

fw64Primitive* fw64_mesh_get_primitive(fw64Mesh* mesh, int index);
fw64Material* fw64_mesh_primitive_get_material(fw64Primitive* primitive);

fw64Texture* fw64_material_get_texture(fw64Material* material);

#ifdef __cplusplus
}
#endif

#endif

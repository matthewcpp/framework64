#pragma once

/** \file mesh_instance.h */

#include "framework64/animation_controller.h"
#include "framework64/box.h"
#include "framework64/mesh.h"

#ifdef FW64_PLATFORM_N64_LIBULTRA
    #include <nusys.h>
#endif

typedef enum {
    FW64_MESH_INSTANCE_FLAG_NONE,
    FW64_MESH_INSTANCE_FLAG_SKINNED
} fw64MeshInstanceFlags;

typedef struct fw64Node fw64Node;

typedef struct {
    fw64Node* node;
    fw64Mesh* mesh;
    fw64MaterialCollection* materials;
    Box render_bounds;
    fw64MeshInstanceFlags flags;
#ifdef FW64_PLATFORM_N64_LIBULTRA
    Mtx n64_matrix;
#endif
} fw64MeshInstance;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_mesh_instance_init(fw64MeshInstance* mesh_instance, fw64Node* node, fw64Mesh* mesh);
void fw64_mesh_instance_update(fw64MeshInstance* mesh_instance);
void fw64_mesh_instance_set_mesh(fw64MeshInstance* mesh_instance, fw64Mesh* mesh);

#define fw64_mesh_instance_get_material_collection(mesh_instance) ((mesh_instance)->materials)
#define fw64_mesh_instance_set_material_collection(mesh_instance, material_collection) (mesh_instance)->materials = (material_collection)

#ifdef __cplusplus
}
#endif

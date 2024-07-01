#pragma once

/** \file mesh_instance.h */

#include "framework64/animation_controller.h"
#include "framework64/box.h"
#include "framework64/mesh.h"

#ifdef FW64_PLATFORM_N64_LIBULTRA
    #include <nusys.h>
#endif

typedef struct fw64Node fw64Node;

typedef struct {
    fw64Node* node;
    fw64Mesh* mesh;
    Box render_bounds;
#ifdef FW64_PLATFORM_N64_LIBULTRA
    Mtx n64_matrix;
#endif
} fw64MeshInstance;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_mesh_instance_init(fw64MeshInstance* mesh_instance, fw64Node* node, fw64Mesh* mesh);
void fw64_mesh_instance_update(fw64MeshInstance* mesh_instance);

#ifdef __cplusplus
}
#endif

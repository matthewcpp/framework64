#include "framework64/mesh_instance.h"

#include "framework64/node.h"

void fw64_mesh_instance_update(fw64MeshInstance* mesh_instance) {
    Box mesh_bounding = fw64_mesh_get_bounding_box(mesh_instance->mesh);
    matrix_transform_box(mesh_instance->node->transform.world_matrix, &mesh_bounding, &mesh_instance->render_bounds);

#ifdef FW64_PLATFORM_N64_LIBULTRA
    guMtxF2L((float (*)[4])mesh_instance->node->transform.world_matrix, &mesh_instance->n64_matrix);
#endif
}

void fw64_mesh_instance_init(fw64MeshInstance* mesh_instance, fw64Node* node, fw64Mesh* mesh) {
    mesh_instance->node = node;
    mesh_instance->mesh = mesh;
    fw64_mesh_instance_update(mesh_instance);
}

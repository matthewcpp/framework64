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
    node->mesh_instance = mesh_instance;
    mesh_instance->node = node;
    mesh_instance->flags = FW64_MESH_INSTANCE_FLAG_NONE;
    fw64_mesh_instance_set_mesh(mesh_instance, mesh);
}

void fw64_mesh_instance_set_mesh(fw64MeshInstance* mesh_instance, fw64Mesh* mesh) {
    // TODO: need to deal /w skinned mesh?
    mesh_instance->mesh = mesh;
    mesh_instance->materials = fw64_mesh_get_material_collection(mesh);
    fw64_mesh_instance_update(mesh_instance);
}

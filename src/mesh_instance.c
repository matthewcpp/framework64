#include "framework64/mesh_instance.h"

void fw64_mesh_instance_update(fw64MeshInstance* mesh_instance) {
    Box mesh_bounding = fw64_mesh_get_bounding_box(mesh_instance->mesh);
    matrix_transform_box(mesh_instance->node->transform.world_matrix, &mesh_bounding, &mesh_instance->render_bounds);

#ifdef FW64_PLATFORM_N64_LIBULTRA
    guMtxF2L((float (*)[4])mesh_instance->node->transform.world_matrix, &mesh_instance->render_matrix);
#endif
}

void fw64_mesh_instance_init(fw64MeshInstance* mesh_instance, fw64Node* node, fw64Mesh* mesh) {
    mesh_instance->component.type_id = FW64_COMPONENT_TYPE_MESH_INSTANCE;
    mesh_instance->node = node;
    mesh_instance->mesh = mesh;
    fw64_mesh_instance_update(mesh_instance);
}

void fw64_mesh_instances_init(fw64MeshInstances* mesh_instances, fw64Allocator* allocator) {
    fw64_sparse_set_init(&mesh_instances->components, sizeof(fw64MeshInstance), allocator);
}

fw64MeshInstance* fw64_mesh_instances_create(fw64MeshInstances* mesh_instances, fw64Node* node, fw64Mesh* mesh, fw64MeshInstanceHandle* out_handle) {
    fw64MeshInstanceHandle handle;
    fw64MeshInstance* mesh_instance = (fw64MeshInstance*) fw64_sparse_set_alloc(&mesh_instances->components, &handle);
    fw64_mesh_instance_init(mesh_instance, node, mesh);

    if (out_handle) {
        *out_handle = handle;
    }

    return mesh_instance;
}

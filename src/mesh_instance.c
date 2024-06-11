#include "framework64/mesh_instance.h"

static void fw64_mesh_instance_update(fw64MeshInstance* mesh_instance) {
    (void)mesh_instance;
}

void fw64_mesh_instances_init(fw64MeshInstances* mesh_instances, fw64Allocator* allocator) {
    fw64_sparse_set_init(&mesh_instances->components, sizeof(fw64MeshInstance), allocator);
}

fw64MeshInstance* fw64_mesh_instances_create(fw64MeshInstances* mesh_instances, fw64Node* node, fw64Mesh* mesh, fw64MeshInstanceHandle* out_handle) {
    fw64MeshInstanceHandle handle;
    fw64MeshInstance* mesh_instance = (fw64MeshInstance*) fw64_sparse_set_alloc(&mesh_instances->components, &handle);

    mesh_instance->node = node;
    mesh_instance->mesh = mesh;
    fw64_mesh_instance_update(mesh_instance);

    if (out_handle) {
        *out_handle = handle;
    }

    return mesh_instance;
}

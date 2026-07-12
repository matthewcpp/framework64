#include "libdragon_mesh.h"

#include <string.h>

typedef struct {
    uint32_t primitive_count;
    uint32_t material_bundle;
} MeshInfo;

fw64Mesh* fw64_mesh_load_from_datasource(fw64AssetDatabase* assets, fw64DataSource* datasource, fw64Allocator* allocator) {
    MeshInfo mesh_info;
    fw64_data_source_read(datasource, &mesh_info, sizeof(MeshInfo), 1);
    fw64MaterialBundle* material_bundle = NULL;

    if (mesh_info.material_bundle) {
        material_bundle = fw64_material_bundle_load_from_datasource(datasource, assets, allocator);

        if (!material_bundle) {
            return NULL;
        }
    }

    fw64Mesh* mesh = fw64_allocator_malloc(allocator, sizeof(fw64Mesh));
    mesh->primitive_count = mesh_info.primitive_count;
    mesh->material_bundle = material_bundle;

    // read bounding box
    fw64_data_source_read(datasource, &mesh->bounding_box, sizeof(Box), 1);

    //read material info
    fw64_material_collection_init_empty(&mesh->material_collection, mesh->primitive_count, allocator);
    fw64_data_source_read(datasource, mesh->material_collection.materials, sizeof(fw64Material*), mesh->primitive_count);

    // allocate and read primitive data
    mesh->primitives = fw64_allocator_malloc(allocator, sizeof(fw64Primitive) * mesh->primitive_count);
    for (uint32_t i = 0; i < mesh->primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;

        // read primitive info
        fw64_data_source_read(datasource, &primitive->info, sizeof(PrimitiveInfo), 1);

        // read vertex data
        primitive->vertex_data = fw64_allocator_memalign(fw64_default_allocator(), 16,  primitive->info.vertex_data_size);
        fw64_data_source_read(datasource, primitive->vertex_data, 1, primitive->info.vertex_data_size);

        // read element data
        primitive->element_data = fw64_allocator_memalign(fw64_default_allocator(), 16, sizeof(GLushort) * primitive->info.element_count);
        fw64_data_source_read(datasource, primitive->element_data, sizeof(GLushort), primitive->info.element_count );

        // fixup materials
        uint32_t material_index =  (uint32_t)mesh->material_collection.materials[i];
        fw64_material_collection_set_material(&mesh->material_collection, i, &mesh->material_bundle->materials[material_index]);
    }

    return mesh;
}

void fw64_mesh_delete(fw64Mesh* mesh, fw64AssetDatabase* assets, fw64Allocator* allocator) {
(void)mesh;
(void)assets;
(void)allocator;

// TODO: implement me
}

Box fw64_mesh_get_bounding_box(fw64Mesh* mesh) {
    return mesh->bounding_box;
}

uint32_t fw64_mesh_get_primitive_count(fw64Mesh* mesh) {
    return mesh->primitive_count;
}

fw64MaterialCollection* fw64_mesh_get_material_collection(fw64Mesh* mesh) {
    return &mesh->material_collection;
}

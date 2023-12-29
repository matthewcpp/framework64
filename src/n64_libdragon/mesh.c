#include "framework64/n64_libdragon/mesh.h"

#include <string.h>

fw64Mesh* fw64_mesh_load_from_datasource(fw64AssetDatabase* assets, fw64DataSource* data_source, fw64Allocator* allocator) {
    fw64Mesh* mesh = allocator->malloc(allocator, sizeof(fw64Mesh));
    fw64_data_source_read(data_source, &mesh->info, sizeof(fw64MeshInfo), 1);

    if (mesh->info.material_bundle) {
        mesh->info.material_bundle = fw64_material_bundle_load_from_datasource(data_source, assets, allocator);
    }

    mesh->primitives = allocator->malloc(allocator, sizeof(fw64Primitive) * mesh->info.primitive_count);

    for (uint32_t i = 0; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;
        memset(&primitive->data, 0 , sizeof(fw64PrimitiveData));

        fw64_data_source_read(data_source, &primitive->info, sizeof(fw64PrimitiveInfo), 1);
        if (primitive->info.vertex_attributes & FW64_PRIMITIVE_ATTRIBUTES_POSITIONS) {
            primitive->data.positions = allocator->malloc(allocator, sizeof(float) * primitive->info.vertex_count * 3);
            fw64_data_source_read(data_source, primitive->data.positions, sizeof(float), primitive->info.vertex_count * 3);
        }
        
        if (primitive->info.vertex_attributes & FW64_PRIMITIVE_ATTRIBUTES_NORMALS) {
            primitive->data.normals = allocator->malloc(allocator, sizeof(float) * primitive->info.vertex_count * 3);
            fw64_data_source_read(data_source, primitive->data.normals, sizeof(float), primitive->info.vertex_count * 3);
        }

        if (primitive->info.vertex_attributes & FW64_PRIMITIVE_ATTRIBUTES_TEXCOORDS) {
            primitive->data.tex_coords = allocator->malloc(allocator, sizeof(float) * primitive->info.vertex_count * 2);
            fw64_data_source_read(data_source, primitive->data.tex_coords, sizeof(float), primitive->info.vertex_count * 2);
        }

        if (primitive->info.vertex_attributes & FW64_PRIMITIVE_ATTRIBUTES_COLORS) {
            primitive->data.colors = allocator->malloc(allocator, primitive->info.vertex_count * 4);
            fw64_data_source_read(data_source, primitive->data.colors, 1, primitive->info.vertex_count * 4);
        }

        const uint32_t element_size = primitive->info.mode == FW64_GLPRIMITIVE_MODE_TRIANGLES ? 3 : 2;
        primitive->data.elements = allocator->malloc(allocator, sizeof(uint16_t) * primitive->info.element_count * element_size);
        fw64_data_source_read(data_source, primitive->data.elements, sizeof(uint16_t), primitive->info.element_count * element_size);

        box_encapsulate_box(&mesh->bounding_box, &primitive->info.bounding_box);
    }

    return mesh;
}


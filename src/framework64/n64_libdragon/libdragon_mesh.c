#include "libdragon_mesh.h"

#include <string.h>

#define VERTEX_SIZE sizeof(float) * 5
#define VERTEX_COUNT 6
#define INDEX_COUNT 6

static GLfloat vertex_data[] = {
    32.0f, 32.0f, 0.0f,      1.0f, 1.0f,
    0.0f, 32.0f, 0.0f,      0.0f, 1.0f,
    0.0f, 0.0f, 0.0f,      0.0f, 0.0f,

    32.0f, 32.0f, 0.0f,      1.0f, 1.0f,
    0.0f, 0.0f, 0.0f,      0.0f, 0.0f,
    32.0f, 0.0f, 0.0f,      1.0f, 0.0f,
};

static GLushort index_data[] = {
    0,1,2,   3,4,5
};

fw64Mesh* fw64_mesh_load_from_datasource(fw64AssetDatabase* assets, fw64DataSource* datasource, fw64Allocator* allocator) {
    fw64MaterialBundle* material_bundle = fw64_material_bundle_load_from_datasource(datasource, assets, allocator);

    if (!material_bundle) {
        return NULL;
    }

    fw64Mesh* mesh = fw64_allocator_malloc(allocator, sizeof(fw64Mesh));
    mesh->primitive_count = 1;
    mesh->material_bundle = material_bundle;

    // allocate and read primitive data
    mesh->primitives = fw64_allocator_malloc(allocator, sizeof(fw64Primitive) * mesh->primitive_count);
    for (uint32_t i = 0; i < mesh->primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;

        primitive->vertex_count = VERTEX_COUNT;
        primitive->vertex_attributes = FW64_VERTEX_ATTRIBUTE_POSITION | FW64_VERTEX_ATTRIBUTE_TEXCOORD;
        primitive->vertex_stride = VERTEX_SIZE;
        primitive->mode = GL_TRIANGLES;
        primitive->element_count = INDEX_COUNT;

        primitive->vertex_data = fw64_allocator_memalign(fw64_default_allocator(), 16, VERTEX_SIZE * VERTEX_COUNT);
        memcpy(primitive->vertex_data, &vertex_data[0], VERTEX_SIZE * VERTEX_COUNT);

        primitive->element_data = fw64_allocator_memalign(fw64_default_allocator(), 16, sizeof(GLushort) * INDEX_COUNT);
        memcpy(primitive->element_data, &index_data[0], sizeof(GLushort) * INDEX_COUNT);
    }

    fw64_material_collection_init_empty(&mesh->material_collection, mesh->primitive_count, allocator);
    fw64_material_collection_set_material(&mesh->material_collection, 0, &mesh->material_bundle->materials[0]);

    vec3_set(&mesh->bounding_box.min, 0.0f, 0.0f, 0.0f);
    vec3_set(&mesh->bounding_box.min, 32.0f, 32.0f, 0.0f);

    return mesh;
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

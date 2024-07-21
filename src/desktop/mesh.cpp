#include "framework64/desktop/mesh.hpp"

#include "framework64/desktop/asset_database.hpp"
#include "framework64/desktop/primitive_data.hpp"
#include "framework64/desktop/shader_cache.hpp"

#include <cassert>

fw64Mesh::fw64Mesh() {
    box_invalidate(&bounding_box);
}

fw64Mesh::~fw64Mesh(){
    for (auto const & primitive : primitives) {
        glDeleteVertexArrays(1, &primitive->gl_info.gl_vertex_array_object);
        glDeleteBuffers(1, &primitive->gl_info.gl_array_buffer_object);
        glDeleteBuffers(1, &primitive->gl_info.gl_element_buffer_object);
    }
}

fw64Primitive* fw64Mesh::createPrimitive(framework64::PrimitiveData&& data, fw64Primitive::Mode primitive_mode) {
    Box primitive_bounding = data.computeBoundingBox();
    return createPrimitive(std::move(data), primitive_mode, primitive_bounding);
}

fw64Primitive* fw64Mesh::createPrimitive(framework64::PrimitiveData&& data, fw64Primitive::Mode primitive_mode, Box const & primitive_bounding) {
    auto& primitive = primitives.emplace_back(std::make_unique<fw64Primitive>());

    primitive->primitive_data = std::move(data);
    primitive->gl_info = primitive->primitive_data.createGlMesh();
    primitive->mode = primitive_mode;
    primitive->bounding_box = primitive_bounding;

    box_encapsulate_box(&bounding_box, &primitive->bounding_box);

    return primitive.get();
}

/// Note this should coorespond to pipeline/desktop/MeshWriter.js _writeMeshInfo
struct MeshInfo {
    uint32_t primitive_count;
    uint32_t material_bundle_count;
    Box bounding_box;
};

/// Note this should coorespond to pipeline/GLMeshWriter.js _writePimitiveInfo
struct PrimitiveInfo {
    uint32_t vertex_count;
    uint32_t vertex_attributes;
    uint32_t element_count;
    uint32_t mode;
    uint32_t material_index; //temp
    uint32_t joint_index;
    Box bounding_box;
};

fw64Mesh* fw64Mesh::loadFromDatasource(fw64DataSource* data_source, fw64MaterialBundle* material_bundle, framework64::ShaderCache& shader_cache, fw64Allocator* allocator) {
    using VertexAttributes = fw64Primitive::Attributes;
    auto mesh = std::make_unique<fw64Mesh>();

    MeshInfo mesh_info;
    fw64_data_source_read(data_source, &mesh_info, sizeof(MeshInfo), 1);

    if (mesh_info.material_bundle_count) {
        mesh->material_bundle.reset(fw64MaterialBundle::loadFromDatasource(data_source, shader_cache, allocator));
        material_bundle = mesh->material_bundle.get();
    }

    mesh->bounding_box = mesh_info.bounding_box;
    mesh->primitives.reserve(mesh_info.primitive_count);

    for (uint32_t i = 0; i < mesh_info.primitive_count; i++) {
        PrimitiveInfo info;
        fw64_data_source_read(data_source, &info, sizeof(info), 1);

        framework64::PrimitiveData primitive_data;

        if (info.vertex_attributes & VertexAttributes::Positions) {
            primitive_data.positions.resize(info.vertex_count * 3);
            fw64_data_source_read(data_source, primitive_data.positions.data(), sizeof(float), info.vertex_count * 3);
        }
        
        if (info.vertex_attributes & VertexAttributes::Normals) {
            primitive_data.normals.resize(info.vertex_count * 3);
            fw64_data_source_read(data_source, primitive_data.normals.data(), sizeof(float), info.vertex_count * 3);
        }

        if (info.vertex_attributes & VertexAttributes::TexCoords) {
            primitive_data.tex_coords.resize(info.vertex_count * 2);
            fw64_data_source_read(data_source, primitive_data.tex_coords.data(), sizeof(float), info.vertex_count * 2);
        }

        if (info.vertex_attributes & VertexAttributes::VertexColors) {
            primitive_data.colors.resize(info.vertex_count * 4);
            fw64_data_source_read(data_source, primitive_data.colors.data(), sizeof(float), info.vertex_count * 4);
        }
        
        const uint32_t element_size = info.mode == fw64Primitive::Mode::Triangles ? 3 : 2;
        primitive_data.indices_array_uint16.resize(info.element_count * element_size);
        fw64_data_source_read(data_source, primitive_data.indices_array_uint16.data(), sizeof(uint16_t), info.element_count * element_size);

        auto* primitive = mesh->createPrimitive(std::move(primitive_data), static_cast<fw64Primitive::Mode>(info.mode), info.bounding_box);
        primitive->joint_index = info.joint_index;
        primitive->material = material_bundle->materials[info.material_index].get();
    }

    return mesh.release();
}

// C interface

fw64Mesh* fw64_mesh_load_from_datasource(fw64AssetDatabase* asset_database, fw64DataSource* data_source, fw64Allocator* allocator) {
    assert(allocator != nullptr);
    return fw64Mesh::loadFromDatasource(data_source, nullptr, asset_database->shader_cache, allocator);
}

fw64Mesh* fw64_mesh_load_from_datasource_with_bundle(fw64AssetDatabase* asset_database, fw64DataSource* data_source, fw64MaterialBundle* material_bundle, fw64Allocator* allocator) {
    assert(allocator != nullptr);
    assert(material_bundle != nullptr);

    return fw64Mesh::loadFromDatasource(data_source, material_bundle, asset_database->shader_cache, allocator);
}

void fw64_mesh_delete(fw64Mesh* mesh, fw64AssetDatabase*, fw64Allocator* allocator) {
    assert(mesh != nullptr);
    assert(allocator != nullptr);
    delete mesh;
}

Box fw64_mesh_get_bounding_box(fw64Mesh* mesh) {
    return mesh->bounding_box;
}

int fw64_mesh_get_primitive_count(fw64Mesh* mesh) {
    return static_cast<int>(mesh->primitives.size());
}

fw64Material* fw64_mesh_get_material_for_primitive(fw64Mesh* mesh, uint32_t index) {
    assert(index < mesh->primitives.size());
    return mesh->primitives[index]->material;
}

fw64PrimitiveMode fw64_mesh_primitive_get_mode(fw64Mesh* mesh, uint32_t index) {
    assert(index < mesh->primitives.size());
    switch (mesh->primitives[index]->mode) {
        case fw64Primitive::Mode::Triangles:
            return FW64_PRIMITIVE_MODE_TRIANGLES;
        
        case fw64Primitive::Mode::Lines:
            return FW64_PRIMITIVE_MODE_LINES;

        case fw64Primitive::Mode::Unknown:
            return FW64_PRIMITIVE_MODE_UNSET;
    }
}

#include "framework64/desktop/mesh_builder.hpp"
#include "framework64/desktop/asset_database.hpp"

#include <cassert>

fw64MeshBuilder::fw64MeshBuilder(fw64AssetDatabase* assets, fw64Allocator* allocator)
    : assets(assets), allocator(allocator) 
{
    box_invalidate(&bounding);
    active_mesh_index = std::numeric_limits<size_t>::max();
}

void fw64MeshBuilder::reset(size_t primitive_count, size_t image_count) {
    assert(primitive_count > 0);

    primitive_info_vec.resize(primitive_count);

    material_bundle = std::make_unique<fw64MaterialBundle>();

    for (size_t i  = 0; i < primitive_count; i++) {
        material_bundle->materials.emplace_back(std::make_unique<fw64Material>(assets->shader_cache));
    }

    active_mesh_index = std::numeric_limits<size_t>::max();
    allocated_image_count = image_count;
}

fw64Texture* fw64MeshBuilder::loadImage(fw64AssetId asset_id) {
    assert(material_bundle->images.size() < allocated_image_count);

    auto* image = fw64_assets_load_image(assets, asset_id, allocator);
    
    if (!image) {
        return nullptr;
    }

    material_bundle->images.emplace_back(image);
    material_bundle->textures.emplace_back(std::make_unique<fw64Texture>(image));

    return material_bundle->textures.back().get();
}

bool fw64MeshBuilder::setActivePrimitive(size_t index) {
    assert(index < primitive_info_vec.size());

    if (index >= primitive_info_vec.size()) {
        return false;
    }

    active_mesh_index = index;

    return true;
}

bool fw64MeshBuilder::allocatePrimitiveData(size_t index, fw64Primitive::Mode mode, fw64VertexAttributes vertex_attributes, size_t vertex_count, size_t element_count) {
    assert(index < primitive_info_vec.size());

    if (index >= primitive_info_vec.size()) {
        return false;
    }

    auto& primitive_info = primitive_info_vec[index];
    primitive_info.mode = mode;
    primitive_info.vertex_attributes = vertex_attributes;

    if (vertex_attributes & FW64_VERTEX_ATTRIBUTE_POSITION) {
        primitive_info.primitive_data.positions.resize(3 * vertex_count);
    }

    if (vertex_attributes & FW64_VERTEX_ATTRIBUTE_NORMAL) {
        primitive_info.primitive_data.normals.resize(3 * vertex_count);
    }

    if (vertex_attributes & FW64_VERTEX_ATTRIBUTE_TEXCOORD) {
        primitive_info.primitive_data.tex_coords.resize(2 * vertex_count);
    }

    if (vertex_attributes & FW64_VERTEX_ATTRIBUTE_COLOR) {
        primitive_info.primitive_data.colors.resize(4 * vertex_count);
    }

    primitive_info.primitive_data.indices_array_uint16.resize(element_count * (mode == fw64Primitive::Mode::Triangles ? 3 : 2));

    return true;
}

bool fw64MeshBuilder::allocatePrimitiveQuadData(size_t primitive_index, fw64VertexAttributes vertex_attributes, size_t count) {
    auto mode = fw64Primitive::Mode::Triangles;
    const size_t vertex_count = count * 4;
    const size_t element_count = count * 2;

    if (!allocatePrimitiveData(primitive_index, mode, vertex_attributes, vertex_count, element_count)) {
        return false;
    }

    auto& primitive_info = primitive_info_vec[primitive_index];

    for (uint16_t i = 0; i < count; i++) {
            uint16_t index = i * 6;
            uint16_t vertex = i * 4;
            primitive_info.primitive_data.indices_array_uint16[index++] = vertex;
            primitive_info.primitive_data.indices_array_uint16[index++] = vertex + 1;
            primitive_info.primitive_data.indices_array_uint16[index++] = vertex + 2;

            primitive_info.primitive_data.indices_array_uint16[index++] = vertex;
            primitive_info.primitive_data.indices_array_uint16[index++] = vertex + 2;
            primitive_info.primitive_data.indices_array_uint16[index++] = vertex + 3;
    }

    return true;
}

fw64Mesh* fw64MeshBuilder::createMesh() {
    assert(material_bundle->images.size() == allocated_image_count);
    auto mesh = std::make_unique<fw64Mesh>();

    mesh->material_bundle = std::move(material_bundle);

    for (auto& material : mesh->material_bundle->materials) {
        assert(material->shading_mode != FW64_SHADING_MODE_UNSET);
        material->shader = assets->shader_cache.getShaderProgram(material->shading_mode);
    }

    for (size_t i = 0; i < primitive_info_vec.size(); i++) {
        auto& primitive_info = primitive_info_vec[i];
        auto* primitive = mesh->createPrimitive(std::move(primitive_info.primitive_data), primitive_info.mode);
        primitive->material = mesh->material_bundle->materials[i].get();
    }

    mesh->bounding_box = bounding;

    primitive_info_vec.clear();

    return mesh.release();
}

// C Interface

fw64MeshBuilder* fw64_mesh_builder_create(fw64AssetDatabase* assets, size_t primitive_count, size_t image_count, fw64Allocator* allocator) {
    auto* mesh_builder =  new fw64MeshBuilder(assets, allocator);
    mesh_builder->reset(primitive_count, image_count);

    return mesh_builder;
}


void fw64_mesh_builder_reset(fw64MeshBuilder* mesh_builder, size_t primitive_count, size_t image_count) {
    mesh_builder->reset(primitive_count, image_count);
}


void fw64_mesh_builder_delete(fw64MeshBuilder* mesh_builder) {
    delete mesh_builder;
}


fw64Texture* fw64_mesh_builder_load_image(fw64MeshBuilder* mesh_builder, fw64AssetId asset_id) {
    return mesh_builder->loadImage(asset_id);
}


fw64Material* fw64_mesh_builder_get_material(fw64MeshBuilder* mesh_builder, size_t index) {
    assert(index < mesh_builder->material_bundle->materials.size());

    return mesh_builder->material_bundle->materials[index].get();
}


int fw64_mesh_builder_allocate_primitive_quad_data(fw64MeshBuilder* mesh_builder, size_t index, fw64VertexAttributes vertex_attributes, size_t count) {
    return mesh_builder->allocatePrimitiveQuadData(index, vertex_attributes, count);
}

int fw64_mesh_builder_allocate_primitive_data(fw64MeshBuilder* mesh_builder, size_t index, fw64PrimitiveMode mode, fw64VertexAttributes vertex_attributes, size_t vertex_count, size_t element_count) {
    auto gl_mode = mode == FW64_PRIMITIVE_MODE_TRIANGLES ? fw64Primitive::Mode::Triangles : fw64Primitive::Mode::Lines;
    return mesh_builder->allocatePrimitiveData(index, gl_mode, vertex_attributes, vertex_count, element_count);
}

void fw64_mesh_builder_set_triangle_vertex_indices(fw64MeshBuilder* mesh_builder, size_t index, uint16_t a, uint16_t b, uint16_t c) {
    auto & active_prim = mesh_builder->primitive_info_vec[mesh_builder->active_mesh_index];

    const size_t element_index = index * 3;
    active_prim.primitive_data.indices_array_uint16[element_index] = a;
    active_prim.primitive_data.indices_array_uint16[element_index + 1] = b;
    active_prim.primitive_data.indices_array_uint16[element_index + 2] = c;
}

void fw64_mesh_builder_set_line_vertex_indices(fw64MeshBuilder* mesh_builder, size_t index, uint16_t a, uint16_t b) {
    auto & active_prim = mesh_builder->primitive_info_vec[mesh_builder->active_mesh_index];

    const size_t element_index = index * 2;
    active_prim.primitive_data.indices_array_uint16[element_index] = a;
    active_prim.primitive_data.indices_array_uint16[element_index + 1] = b;
}

int fw64_mesh_builder_set_active_primitive(fw64MeshBuilder* mesh_builder, size_t index) {
    return mesh_builder->setActivePrimitive(index);
}

void fw64_mesh_builder_set_vertex_position_f(fw64MeshBuilder* mesh_builder, size_t index, float x, float y, float z) {
    auto & active_prim = mesh_builder->primitive_info_vec[mesh_builder->active_mesh_index];
    size_t pos_index = index * 3;
    active_prim.primitive_data.positions[pos_index++] = x;
    active_prim.primitive_data.positions[pos_index++] = y;
    active_prim.primitive_data.positions[pos_index++] = z;
}

void fw64_mesh_builder_set_vertex_position_int16(fw64MeshBuilder* mesh_builder, size_t index, int16_t x, int16_t y, int16_t z) {
    auto & active_prim = mesh_builder->primitive_info_vec[mesh_builder->active_mesh_index];
    size_t pos_index = index * 3;

    active_prim.primitive_data.positions[pos_index++] = static_cast<float>(x);
    active_prim.primitive_data.positions[pos_index++] = static_cast<float>(y);
    active_prim.primitive_data.positions[pos_index++] = static_cast<float>(z);
}

void fw64_mesh_builder_set_vertex_normal_f(fw64MeshBuilder* mesh_builder, size_t index, float x, float y, float z) {
    auto & active_prim = mesh_builder->primitive_info_vec[mesh_builder->active_mesh_index];
    size_t pos_index = index * 3;
    active_prim.primitive_data.normals[pos_index++] = x;
    active_prim.primitive_data.normals[pos_index++] = y;
    active_prim.primitive_data.normals[pos_index++] = z;
}

void fw64_mesh_builder_set_vertex_texcoords_f(fw64MeshBuilder* mesh_builder, size_t index, float s, float t) {
    auto & active_prim = mesh_builder->primitive_info_vec[mesh_builder->active_mesh_index];
    size_t pos_index = index * 2;
    active_prim.primitive_data.tex_coords[pos_index++] = s;
    active_prim.primitive_data.tex_coords[pos_index++] = t;
}

void fw64_mesh_builder_set_vertex_color_rgba8(fw64MeshBuilder* mesh_builder, size_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    fw64ColorRGBA8 color = {r, g, b, a};
    fw64_mesh_builder_set_vertex_color_c(mesh_builder, index, color);
}

void fw64_mesh_builder_set_vertex_color_c(fw64MeshBuilder* mesh_builder, size_t index, fw64ColorRGBA8 color) {
    auto & active_prim = mesh_builder->primitive_info_vec[mesh_builder->active_mesh_index];
    size_t pos_index = index * 4;
    
    //premultiply alpha value
    float alpha = static_cast<float>(color.a) / 255.0f;

    active_prim.primitive_data.colors[pos_index++] = (static_cast<float>(color.r) / 255.0f) * alpha;
    active_prim.primitive_data.colors[pos_index++] = (static_cast<float>(color.g) / 255.0f) * alpha;
    active_prim.primitive_data.colors[pos_index++] = (static_cast<float>(color.b) / 255.0f) * alpha;
    active_prim.primitive_data.colors[pos_index++] = alpha;
}

void fw64_mesh_builder_set_bounding(fw64MeshBuilder* mesh_builder, const Box* bounding) {
    mesh_builder->bounding = *bounding;
}

fw64Mesh* fw64_mesh_builder_commit(fw64MeshBuilder* mesh_builder) {
    return mesh_builder->createMesh();
}
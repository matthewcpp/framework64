#include "framework64/util/quad.h"

#include "framework64/desktop/engine.h"
#include "framework64/desktop/texture.h"
#include "framework64/vec3.h"
#include "framework64/desktop/mesh.h"
#include "framework64/desktop/mesh_data.h"

#include <vector>

fw64Mesh* textured_quad_create(fw64Engine* engine, fw64Texture* texture) {
    auto* f64_engine = reinterpret_cast<framework64::Engine*>(engine);
    uint32_t slice_count = texture->hslices * texture->vslices;

    auto* mesh = new fw64Mesh();

    Vec3 min_pt = {-1.0f, -1.0f, 0.0f};
    Vec3 max_pt = {1.0f, -.0f, 0.0f};

    if(slice_count > 1) {
        min_pt = {-texture->hslices / 2.0f, -texture->vslices / 2.0f, 0.0f};
        max_pt = {texture->hslices / 2.0f, texture->vslices / 2.0f, 0.0f};
    }

    framework64::MeshData mesh_data;

    mesh_data.positions = {
        min_pt.x, max_pt.y, 0.0f,
        max_pt.x, max_pt.y, 0.0f,
        max_pt.x, min_pt.y, 0.0f,
        min_pt.x, min_pt.y, 0.0f
    };

    mesh_data.normals = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };

    mesh_data.tex_coords = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    mesh_data.indices_array_uint16 = {
        0, 1, 2,
        0, 2, 3
    };

    auto& primitive = mesh->primitives.emplace_back();
    auto mesh_info = mesh_data.createMesh();
    mesh_info.setPrimitiveValues(primitive);
    primitive.mode = fw64Mesh::Primitive::Mode::Triangles;
    primitive.material.texture = texture;
    f64_engine->shader_cache->setShaderProgram(primitive);
    return mesh;
}

void textured_quad_set_tex_coords(fw64Mesh* mesh, int frame, float s, float t) {

}

fw64Mesh* quad_create(int16_t size, Color* color) {
    return nullptr;
}
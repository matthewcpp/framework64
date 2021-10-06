#include "framework64/util/quad.h"

#include "framework64/desktop/engine.h"
#include "framework64/desktop/mesh_data.h"

#include <vector>

static framework64::MeshData make_mesh_data(Vec3 const & min_pt, Vec3 const & max_pt, float max_s, float max_t);
static fw64Mesh* create_mesh(framework64::Engine* f64_engine, framework64::MeshData & mesh_data, fw64Image* image);

fw64Mesh* textured_quad_create_with_params(fw64Engine* engine, int image_asset_index, float max_s, float max_t) {
    auto* f64_engine = reinterpret_cast<framework64::Engine*>(engine);
    auto* image = fw64_image_load(engine->assets, image_asset_index);
    uint32_t slice_count = image->hslices * image->vslices;

    Vec3 min_pt = {-1.0f, -1.0f, 0.0f};
    Vec3 max_pt = {1.0f, 1.0f, 0.0f};

    if(slice_count > 1) {
        min_pt = {-image->hslices / 2.0f, -image->vslices / 2.0f, 0.0f};
        max_pt = {image->hslices / 2.0f, image->vslices / 2.0f, 0.0f};
    }

    auto mesh_data = make_mesh_data(min_pt, max_pt, max_s, max_t);
    return create_mesh(f64_engine, mesh_data, image);

}

fw64Mesh* textured_quad_create_with_image(fw64Engine* engine, fw64Image* image, int frame_index) {
    auto* f64_engine = reinterpret_cast<framework64::Engine*>(engine);

    Vec3 min_pt = {-1.0f, -1.0f, 0.0f};
    Vec3 max_pt = {1.0f, 1.0f, 0.0f};

    auto mesh_data = make_mesh_data(min_pt, max_pt, 1.0f, 1.0f);
    auto* mesh = create_mesh(f64_engine, mesh_data, image);

    mesh->primitives[0].material.texture_frame = frame_index;
    f64_engine->shader_cache->setParticleShader(mesh->primitives[0]);
    return mesh;
}

framework64::MeshData make_mesh_data(Vec3 const & min_pt, Vec3 const & max_pt, float max_s, float max_t) {
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
            max_s, 0.0f,
            max_s, max_t,
            0.0f, max_t
    };

    mesh_data.indices_array_uint16 = {
            0, 1, 2,
            0, 2, 3
    };

    return mesh_data;
}

fw64Mesh* create_mesh(framework64::Engine* f64_engine, framework64::MeshData & mesh_data, fw64Image* image) {
    auto* mesh = new fw64Mesh();
    mesh->textures.emplace_back(new fw64Texture(image));

    auto& primitive = mesh->primitives.emplace_back();
    auto mesh_info = mesh_data.createMesh();
    mesh_info.setPrimitiveValues(primitive);
    primitive.mode = fw64Primitive::Mode::Triangles;
    primitive.material.texture = mesh->textures[0].get();
    f64_engine->shader_cache->setShaderProgram(primitive);
    return mesh;
}

fw64Mesh* textured_quad_create(fw64Engine* engine, int image_asset_index) {
    return textured_quad_create_with_params(engine, image_asset_index, 1.0f, 1.0f);
}
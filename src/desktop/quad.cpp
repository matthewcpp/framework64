#include "framework64/util/quad.h"

#include "framework64/desktop/engine.h"
#include "framework64/desktop/mesh_data.h"

#include <vector>

static fw64Mesh* create_mesh(framework64::Engine* f64_engine, framework64::MeshData & mesh_data, fw64Image* image);

static framework64::MeshData make_mesh_data(Vec3 const & min_pt, Vec3 const & max_pt, float min_s, float max_s, float min_t, float max_t) {
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
            min_s, min_t,
            max_s, min_t,
            max_s, max_t,
            min_s, max_t
    };

    mesh_data.indices_array_uint16 = {
            0, 1, 2,
            0, 2, 3
    };

    return mesh_data;
}

fw64Mesh* create_mesh(framework64::Engine* f64_engine, framework64::MeshData & mesh_data, fw64Image* image) {
    auto* mesh = new fw64Mesh();
    mesh->resources = std::make_unique<framework64::SharedResources>();
    mesh->resources->textures.emplace_back(new fw64Texture(image));

    auto& primitive = mesh->primitives.emplace_back();
    auto mesh_info = mesh_data.createMesh();
    mesh_info.setPrimitiveValues(primitive);
    primitive.mode = fw64Primitive::Mode::Triangles;
    primitive.material.texture = mesh->resources->textures[0].get();
    mesh->primitives[0].material.shader = f64_engine->shader_cache->getSpriteShaderProgram();

    return mesh;
}

fw64Mesh* fw64_textured_quad_create(fw64Engine* engine, int image_asset_index, fw64Allocator* allocator) {
    fw64TexturedQuadParams params;
    fw64_textured_quad_params_init(&params);
    params.image_asset_index = image_asset_index;
    params.is_animated = 0;

    fw64Mesh* mesh = fw64_textured_quad_create_with_params(engine, &params, allocator);

    return mesh;
}

static fw64Mesh* fw64_textured_quad_create_animated(fw64Engine* engine, fw64TexturedQuadParams* params, fw64Allocator* allocator) {
    auto* f64_engine = reinterpret_cast<framework64::Engine*>(engine);
    fw64Image* image = params->image;

    if (!image)
        image = fw64_image_load(engine->assets, params->image_asset_index, allocator);

    Vec3 min_pt = {-1.0f, -1.0f, 0.0f};
    Vec3 max_pt = {1.0f, 1.0f, 0.0f};

    auto mesh_data = make_mesh_data(min_pt, max_pt, params->min_s, params->max_s, params->min_t, params->max_t);
    auto* mesh = create_mesh(f64_engine, mesh_data, image);

    mesh->primitives[0].material.texture_frame = 0;

    return mesh;
}

static fw64Mesh* fw64_textured_quad_create_static(fw64Engine* engine, fw64TexturedQuadParams* params, fw64Allocator* allocator) {
    auto* f64_engine = reinterpret_cast<framework64::Engine*>(engine);
    fw64Image* image = params->image;

    if (!image)
        image = fw64_image_load(engine->assets, params->image_asset_index, allocator);

    uint32_t slice_count = image->hslices * image->vslices;

    Vec3 min_pt = {-1.0f, -1.0f, 0.0f};
    Vec3 max_pt = {1.0f, 1.0f, 0.0f};

    if (slice_count > 1) {
        min_pt = {-image->hslices / 2.0f, -image->vslices / 2.0f, 0.0f};
        max_pt = {image->hslices / 2.0f, image->vslices / 2.0f, 0.0f};
    }

    auto mesh_data = make_mesh_data(min_pt, max_pt, params->min_s, params->max_s, params->min_t, params->max_t);
    auto* mesh = create_mesh(f64_engine, mesh_data, image);

    mesh->primitives[0].material.texture_frame = FW64_DESKTOP_ENTIRE_TEXTURE_FRAME;

    return mesh;
}

fw64Mesh* fw64_textured_quad_create_with_params(fw64Engine* engine, fw64TexturedQuadParams* params, fw64Allocator* allocator) {
    if (params->is_animated)
        return fw64_textured_quad_create_animated(engine, params, allocator);
    else
        return fw64_textured_quad_create_static(engine, params, allocator);
}
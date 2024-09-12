#include "framework64/util/quad.h"
#include "framework64/util/mesh_builder.h"

void fw64_textured_quad_params_init(fw64TexturedQuadParams* params) {
    params->texture = NULL;
    params->image_asset_id = FW64_INVALID_ASSET_ID;
    params->shading_mode = FW64_SHADING_MODE_UNLIT_TEXTURED;
    params->min_s = 0.0f;
    params->max_s = 1.0f;
    params->min_t = 0.0f;
    params->max_t = 1.0f;
}

fw64Mesh* fw64_textured_quad_create(fw64Engine* engine, fw64AssetId image_asset_id, fw64Allocator* allocator) {
    fw64TexturedQuadParams params;
    fw64_textured_quad_params_init(&params);
    params.image_asset_id = image_asset_id;

    fw64Mesh* mesh = fw64_textured_quad_create_with_params(engine, &params, allocator);

    return mesh;
}

fw64Mesh* fw64_textured_quad_create_with_params(fw64Engine* engine, fw64TexturedQuadParams* params, fw64Allocator* allocator) {
    size_t load_image = params->texture ? 0 : 1;
    fw64Texture* texture = NULL;

    fw64MeshBuilder* mesh_builder = fw64_mesh_builder_create(engine->assets, 1, load_image, allocator);

    if (load_image) {
        texture = fw64_mesh_builder_load_image(mesh_builder, params->image_asset_id);
    } else {
        texture = params->texture;
    }

    fw64Material* material = fw64_mesh_builder_get_material(mesh_builder, 0);
    fw64_material_set_shading_mode(material, params->shading_mode);
    fw64_material_set_texture(material, texture, 0);

    fw64VertexAttributes attributes = FW64_VERTEX_ATTRIBUTE_POSITION | FW64_VERTEX_ATTRIBUTE_TEXCOORD | FW64_VERTEX_ATTRIBUTE_COLOR;
    fw64_mesh_builder_allocate_primitive_quad_data(mesh_builder, 0, attributes, 1);
    fw64_mesh_builder_set_active_primitive(mesh_builder, 0);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 0, -1.0f, -1.0f, 0.0f);
    fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, 0, params->min_s, params->max_t);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 0, 255, 255, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 1, 1.0f, -1.0f, 0.0f);
    fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, 1, params->max_s, params->max_t);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 1, 255, 255, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 2, 1.0f, 1.0f, 0.0f);
    fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, 2, params->max_s, params->min_t);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 2, 255, 255, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 3, -1.0f, 1.0f, 0.0f);
    fw64_mesh_builder_set_vertex_texcoords_f(mesh_builder, 3, params->min_s, params->min_t);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 3, 255, 255, 255, 255);

    Box bounding = {{-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}};
    fw64_mesh_builder_set_bounding(mesh_builder, &bounding);  

    fw64Mesh* mesh = fw64_mesh_builder_commit(mesh_builder);
    fw64_mesh_builder_delete(mesh_builder);

    return mesh;
}

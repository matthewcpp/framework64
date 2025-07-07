#include "fw64_wireframe.h"

#include "framework64/util/mesh_builder.h"

fw64Mesh* fw64_wireframe_build_box(const Box* box, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    fw64MeshBuilder* mesh_builder = fw64_mesh_builder_create(assets, 1, 0, allocator);
    fw64VertexAttributes attributes = FW64_VERTEX_ATTRIBUTE_POSITION | FW64_VERTEX_ATTRIBUTE_COLOR;
    fw64_mesh_builder_allocate_primitive_data(mesh_builder, 0, FW64_PRIMITIVE_MODE_LINES, attributes, 8, 12);
    fw64_mesh_builder_set_active_primitive(mesh_builder, 0);
    fw64_material_set_shading_mode(fw64_mesh_builder_get_material(mesh_builder, 0), FW64_SHADING_MODE_LINE);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 0, box->min.x, box->max.y, box->min.z);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder,0, 255, 255, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 1, box->max.x, box->max.y, box->min.z);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder,1, 255, 255, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 2, box->max.x, box->max.y, box->max.z);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder,2, 255, 255, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 3, box->min.x, box->max.y, box->max.z);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder,3, 255, 255, 255, 255);


    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 4, box->min.x, box->min.y,  box->min.z);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder,4, 255, 255, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 5, box->max.x, box->min.y,  box->min.z);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder,5, 255, 255, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 6, box->max.x, box->min.y, box->max.z);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder,6, 255, 255, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 7, box->min.x, box->min.y, box->max.z);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder,7, 255, 255, 255, 255);

    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 0, 0, 1);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 1, 1, 2);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 2, 2, 3);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 3, 3, 0);

    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 4, 4, 5);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 5, 5, 6);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 6, 6, 7);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 7, 7, 4);

    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 8, 0, 4);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 9, 1, 5);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 10, 2, 6);
    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 11, 3, 7);

    Box bounding = {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    fw64_mesh_builder_set_bounding(mesh_builder, &bounding);

    fw64Mesh* mesh = fw64_mesh_builder_commit(mesh_builder);
    fw64_mesh_builder_delete(mesh_builder);

    return mesh;
}

fw64Mesh* fw64_wireframe_build_collision_mesh(const fw64CollisionMesh* collision_mesh, fw64AssetDatabase* assets, fw64Allocator* allocator) {
    fw64MeshBuilder* mesh_builder = fw64_mesh_builder_create(assets, 1, 0, allocator);
    fw64VertexAttributes attributes = FW64_VERTEX_ATTRIBUTE_POSITION | FW64_VERTEX_ATTRIBUTE_COLOR;
    fw64_mesh_builder_allocate_primitive_data(mesh_builder, 0, FW64_PRIMITIVE_MODE_LINES, attributes, collision_mesh->point_count, collision_mesh->element_count * 3);
    fw64_mesh_builder_set_active_primitive(mesh_builder, 0);
    fw64_material_set_shading_mode(fw64_mesh_builder_get_material(mesh_builder, 0), FW64_SHADING_MODE_LINE);

    for (size_t i = 0; i < collision_mesh->point_count; i++) {
        const Vec3* pt = collision_mesh->points + i;

        fw64_mesh_builder_set_vertex_position_f(mesh_builder, i, pt->x, pt->y, pt->z);
        fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder,i, 255, 255, 255, 255);
    }

    const uint16_t triangle_count = (uint16_t)fw64_collision_mesh_get_triangle_count(collision_mesh);
    size_t line_index = 0;
    for (uint16_t i = 0; i < triangle_count; i++) {
        const uint16_t index = i * 3;
        fw64_mesh_builder_set_line_vertex_indices(mesh_builder, line_index++, index, index + 1);
        fw64_mesh_builder_set_line_vertex_indices(mesh_builder, line_index++, index, index + 2);
        fw64_mesh_builder_set_line_vertex_indices(mesh_builder, line_index++, index + 1, index + 2);
    }

    fw64Mesh* mesh = fw64_mesh_builder_commit(mesh_builder);
    fw64_mesh_builder_delete(mesh_builder);

    return mesh;
}

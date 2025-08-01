#include "fw64_debug_character.h"

#include <framework64/util/mesh_builder.h>

void fw64_debug_character_init(fw64DebugCharacter* debug, fw64Character* character){
    debug->character = character;

    fw64MeshBuilder* mesh_builder = fw64_mesh_builder_create(character->scene->assets, 1, 0, character->scene->allocator);
    fw64VertexAttributes attributes = FW64_VERTEX_ATTRIBUTE_POSITION | FW64_VERTEX_ATTRIBUTE_COLOR;
    fw64_mesh_builder_allocate_primitive_data(mesh_builder, 0, FW64_PRIMITIVE_MODE_LINES, attributes, 2, 1);
    fw64_mesh_builder_set_active_primitive(mesh_builder, 0);
    fw64_material_set_shading_mode(fw64_mesh_builder_get_material(mesh_builder, 0), FW64_SHADING_MODE_LINE);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 0, 0.0f, 0.0f, 0.0f);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 0, 0, 0, 255, 255);

    fw64_mesh_builder_set_vertex_position_f(mesh_builder, 1, 0.0f, -5.0f, 0.0f);
    fw64_mesh_builder_set_vertex_color_rgba8(mesh_builder, 1, 0, 0, 255, 255);

    fw64_mesh_builder_set_line_vertex_indices(mesh_builder, 0, 0, 1);

    fw64Mesh* ledge_grab_mesh = fw64_mesh_builder_commit(mesh_builder);
    fw64_mesh_builder_delete(mesh_builder);

    fw64Node* ledge_grab_node = fw64_scene_create_node(character->scene);
    debug->ledge_grab_mesh_instance = fw64_scene_create_mesh_instance(character->scene, ledge_grab_node, ledge_grab_mesh);
}

void fw64_debug_character_update(fw64DebugCharacter* debug) {
    fw64_character_get_ledge_check_origin(debug->character, &debug->ledge_grab_mesh_instance->node->transform.position);
    fw64_transform_update_matrix(&debug->ledge_grab_mesh_instance->node->transform);
    fw64_mesh_instance_update(debug->ledge_grab_mesh_instance);
}

void fw64_debug_character_draw(fw64DebugCharacter* debug, fw64RenderPass* renderpass) {
    fw64_renderpass_draw_static_mesh(renderpass, debug->ledge_grab_mesh_instance);
}

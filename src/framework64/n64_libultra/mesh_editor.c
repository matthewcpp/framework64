#include "framework64/util/mesh_editor.h"
#include "framework64/n64_libultra/mesh.h"
#include "framework64/n64_libultra/vertex.h"

void fw64_mesh_editor_init(fw64MeshEditor* mesh_editor, fw64Mesh* mesh) {
    mesh_editor->mesh = mesh;
    mesh_editor->primitive = NULL;
    mesh_editor->flags = 0;

    fw64_mesh_editor_set_active_primitive(mesh_editor, 0);
}

int fw64_mesh_editor_set_active_primitive(fw64MeshEditor* mesh_editor, size_t index) {
    if (index >= mesh_editor->mesh->info.primitive_count) {
        return 0;
    }

    mesh_editor->prim_index = index;
    mesh_editor->primitive = mesh_editor->mesh->primitives + index;

    return 1;
}

void fw64_mesh_editor_commit(fw64MeshEditor* editor) {
    // Nothing specific needs to be done here
    (void)editor;
}

void fw64_mesh_editor_set_vertex_positionf(fw64MeshEditor* mesh_editor, size_t index, float x, float y, float z) {
    fw64_n64_vertex_set_position_f(mesh_editor->primitive->vertices + index, x, y, z);
}

void fw64_mesh_editor_set_vertex_normal_f(fw64MeshEditor* mesh_editor, size_t index, float x, float y, float z) {
    fw64_n64_vertex_set_normal_f(mesh_editor->primitive->vertices + index, x, y, z);
}

void fw64_mesh_editor_set_vertex_color_rgba8(fw64MeshEditor* mesh_editor, size_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    fw64_n64_vertex_set_color_rgba8(mesh_editor->primitive->vertices + index, r, g, b, a);
}

void fw64_mesh_editor_set_vertex_texcoords_f(fw64MeshEditor* mesh_editor, size_t index, float s, float t) {
    fw64Material* material = fw64_material_collection_get_material(&mesh_editor->mesh->material_collection, mesh_editor->prim_index);
    fw64_n64_vertex_set_texcoords_f(mesh_editor->primitive->vertices + index, material->texture, s, t);
}

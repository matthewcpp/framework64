#include "framework64/util/mesh_editor.h"
#include "framework64/n64/mesh.h"
#include "framework64/n64/vertex.h"

void fw64_mesh_editor_init(fw64MeshEditor* mesh_editor, fw64Mesh* mesh) {
    mesh_editor->mesh = mesh;
    mesh_editor->primitive = NULL;
    mesh_editor->flags = 0;
}

int fw64_mesh_editor_set_active_primitive(fw64MeshEditor* mesh_editor, size_t index) {
    if (index >= mesh_editor->mesh->info.primitive_count) {
        return 0;
    }

    mesh_editor->primitive = mesh_editor->mesh->primitives + index;

    return 1;
}

void fw64_mesh_editor_commit(fw64MeshEditor* mesh_editor) {
    // Nothing specific needs to be done here
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
    fw64_n64_vertex_set_texcoords_f(mesh_editor->primitive->vertices + index, mesh_editor->primitive->material->texture, s, t);
}

#include "framework64/util/mesh_editor.h"

#include "framework64/desktop/mesh.hpp"

#include <cassert>

void fw64_mesh_editor_init(fw64MeshEditor* mesh_editor, fw64Mesh* mesh) {
    assert(mesh != nullptr);
    assert(mesh->primitives.size() > 0);

    mesh_editor->mesh = mesh;
    mesh_editor->primitive = nullptr;
    mesh_editor->flags = 0;

    fw64_mesh_editor_set_active_primitive(mesh_editor, 0);
}

int fw64_mesh_editor_set_active_primitive(fw64MeshEditor* mesh_editor, size_t index) {
    assert(mesh_editor->flags == 0);

    if (index >= mesh_editor->mesh->primitives.size()) {
        return 0;
    }

    mesh_editor->primitive = mesh_editor->mesh->primitives[index].get();

    return 1;
}

void fw64_mesh_editor_commit(fw64MeshEditor* mesh_editor) {
    if (mesh_editor->flags == 0 || mesh_editor->primitive == NULL) {
        return;
    }

    GLsizeiptr buffer_offset = 0;
    glBindBuffer(GL_ARRAY_BUFFER, mesh_editor->primitive->gl_info.gl_array_buffer_object);
    const auto& primitive_data = mesh_editor->primitive->primitive_data;
    
    if (mesh_editor->primitive->gl_info.attributes & fw64Primitive::Attributes::Positions) {
        GLsizeiptr data_size = primitive_data.positions.size() * sizeof(float);

        if (mesh_editor->flags & FW64_VERTEX_ATTRIBUTE_POSITION) {
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, primitive_data.positions.data());
        }

        buffer_offset += data_size;
    }

    if (mesh_editor->primitive->gl_info.attributes & fw64Primitive::Attributes::Normals) {
        GLsizeiptr data_size = primitive_data.normals.size() * sizeof(float);

        if (mesh_editor->flags & FW64_VERTEX_ATTRIBUTE_NORMAL) {
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, primitive_data.normals.data());
        }

        buffer_offset += data_size;
    }

    if (mesh_editor->primitive->gl_info.attributes & fw64Primitive::Attributes::TexCoords) {
        GLsizeiptr data_size = primitive_data.tex_coords.size() * sizeof(float);

        if (mesh_editor->flags & FW64_VERTEX_ATTRIBUTE_TEXCOORD) {
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, primitive_data.tex_coords.data());
        }

        buffer_offset += data_size;
    }

    if (mesh_editor->primitive->gl_info.attributes & fw64Primitive::Attributes::VertexColors) {
        GLsizeiptr data_size = primitive_data.colors.size() * sizeof(float);

        if (mesh_editor->flags & FW64_VERTEX_ATTRIBUTE_COLOR) {
            glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, primitive_data.colors.data());
        }

        buffer_offset += data_size;
    }

    mesh_editor->flags = 0;
}

void fw64_mesh_editor_set_vertex_positionf(fw64MeshEditor* mesh_editor, size_t index, float x, float y, float z) {
    size_t pos_index = index * 3;

    mesh_editor->primitive->primitive_data.positions[pos_index++] = x;
    mesh_editor->primitive->primitive_data.positions[pos_index++] = y;
    mesh_editor->primitive->primitive_data.positions[pos_index++] = z;

    mesh_editor->flags |= FW64_VERTEX_ATTRIBUTE_POSITION;
}

void fw64_mesh_editor_set_vertex_normal_f(fw64MeshEditor* mesh_editor, size_t index, float x, float y, float z) {
    size_t pos_index = index * 3;

    mesh_editor->primitive->primitive_data.normals[pos_index++] = x;
    mesh_editor->primitive->primitive_data.normals[pos_index++] = y;
    mesh_editor->primitive->primitive_data.normals[pos_index++] = z;

    mesh_editor->flags |= FW64_VERTEX_ATTRIBUTE_NORMAL;
}

void fw64_mesh_editor_set_vertex_color_rgba8(fw64MeshEditor* mesh_editor, size_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    size_t pos_index = index * 4;

    mesh_editor->primitive->primitive_data.colors[pos_index++] = static_cast<float>(r) / 255.0f;
    mesh_editor->primitive->primitive_data.colors[pos_index++] = static_cast<float>(g) / 255.0f;
    mesh_editor->primitive->primitive_data.colors[pos_index++] = static_cast<float>(b) / 255.0f;
    mesh_editor->primitive->primitive_data.colors[pos_index++] = static_cast<float>(a) / 255.0f;
    
    mesh_editor->flags |= FW64_VERTEX_ATTRIBUTE_COLOR;
}

void fw64_mesh_editor_set_vertex_texcoords_f(fw64MeshEditor* mesh_editor, size_t index, float s, float t) {
    size_t pos_index = index * 2;

    mesh_editor->primitive->primitive_data.tex_coords[pos_index++] = s;
    mesh_editor->primitive->primitive_data.tex_coords[pos_index++] = t;

    mesh_editor->flags |= FW64_VERTEX_ATTRIBUTE_TEXCOORD;
}

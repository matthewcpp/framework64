#include "framework64/desktop/mesh_data.h"

#include <algorithm>
#include <iterator>
#include <unordered_map>

namespace framework64 {

GLMeshInfo MeshData::createGlMesh() {
    GLMeshInfo mesh_info;

    GLuint array_buffer_size = (positions.size() + normals.size() + tex_coords.size() + colors.size()) * sizeof(float);
    glGenVertexArrays(1, &mesh_info.gl_vertex_array_object);
    glBindVertexArray(mesh_info.gl_vertex_array_object);

    glGenBuffers(1, &mesh_info.gl_array_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_info.gl_array_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, array_buffer_size, nullptr, GL_STATIC_DRAW);
    GLsizeiptr buffer_offset = 0;

    if (!positions.empty()) {
        GLsizeiptr data_size = positions.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, positions.data());
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
        buffer_offset += data_size;
        mesh_info.attributes |= fw64Primitive::Attributes::Positions;
    }

    if (!normals.empty()) {
        GLsizeiptr data_size = normals.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, normals.data());
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
        buffer_offset += data_size;
        mesh_info.attributes |= fw64Primitive::Attributes::Normals;
    }

    if (!tex_coords.empty()) {
        GLsizeiptr data_size = tex_coords.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, tex_coords.data());
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
        buffer_offset += data_size;
        mesh_info.attributes |= fw64Primitive::Attributes::TexCoords;
    }

    // note that we are assuming vertex colors are encoded as RGB floats (seems to be default blender export)
    if (!colors.empty()) {
        GLsizeiptr data_size = colors.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, data_size, colors.data());
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)buffer_offset);
        buffer_offset += data_size;

        mesh_info.attributes |= fw64Primitive::Attributes::VertexColors;
    }

    if (!indices_array_uint16.empty()) {
        glGenBuffers(1, &mesh_info.gl_element_buffer_object);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_info.gl_element_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_array_uint16.size() * sizeof(uint16_t), indices_array_uint16.data(), GL_STATIC_DRAW);
        mesh_info.primitive_mode = GL_UNSIGNED_SHORT;
        mesh_info.element_count = static_cast<GLsizei>(indices_array_uint16.size());
    }
    else if (!indices_array_uint32.empty()) {
        glGenBuffers(1, &mesh_info.gl_element_buffer_object);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_info.gl_element_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_array_uint32.size() * sizeof(uint32_t), indices_array_uint32.data(), GL_STATIC_DRAW);
        mesh_info.primitive_mode = GL_UNSIGNED_INT;
        mesh_info.element_count = static_cast<GLsizei>(indices_array_uint32.size());
    }


    glBindVertexArray(0);

    return mesh_info;
}

bool MeshData::hasMultipleJointIndices() {
    if (joint_indices.empty())
        return false;

    uint8_t joint = joint_indices[0];
    // note: these are read in as VEC4 with type unsigned byte...current implementation supports only a single joint
    for (size_t i = 4; i < joint_indices.size(); i += 4) {
        if (joint_indices[i] != joint)
            return true;
    }

    return false;
}

void MeshData::moveMeshDataToPrimitive(fw64Primitive& primitive) {
    primitive.positions = std::move(positions);
    primitive.normals = std::move(normals);
    primitive.tex_coords = std::move(tex_coords);
    primitive.colors = std::move(colors);

    if (!indices_array_uint16.empty()) {
        primitive.indices = std::move(indices_array_uint16);
    }
    else if (!indices_array_uint32.empty()) {
        std::transform(
            indices_array_uint32.begin(), indices_array_uint32.end(), 
            std::back_inserter(primitive.indices),
            [](uint32_t index) -> uint16_t {
                return static_cast<uint16_t>(index);
            });
    }
}

std::vector<MeshData> MeshData::splitByJointIndex() {
    std::vector<MeshData> split_data;

    return split_data;
}

void GLMeshInfo::setPrimitiveValues(fw64Primitive& primitive) {
    primitive.gl_array_buffer_object = gl_array_buffer_object;
    primitive.gl_vertex_array_object = gl_vertex_array_object;
    primitive.gl_element_buffer_object = gl_element_buffer_object;

    primitive.attributes = attributes;
    primitive.primitive_mode = primitive_mode;
    primitive.element_count = element_count;
}

}
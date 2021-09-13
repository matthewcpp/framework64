#pragma once

#include "framework64/desktop/mesh.h"

#include <gl/glew.h>

#include <cstdint>
#include <vector>

namespace framework64 {

struct GLMeshInfo {
    uint32_t gl_vertex_array_object = 0;
    uint32_t gl_array_buffer_object = 0;
    uint32_t gl_element_buffer_object = 0;

    uint32_t attributes = 0;
    GLenum element_type = 0;

    void setPrimitiveValues(fw64Mesh::Primitive& primitive);
};

struct MeshData {
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> tex_coords;
    std::vector<float> colors;

    std::vector<uint16_t> indices_array_uint16;
    std::vector<uint32_t> indices_array_uint32;

    GLMeshInfo createMesh();
};



}
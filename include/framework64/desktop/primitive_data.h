#pragma once

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <cstdint>
#include <vector>

#include "framework64/box.h"

namespace framework64 {

struct GLMeshInfo {
    uint32_t gl_vertex_array_object = 0;
    uint32_t gl_array_buffer_object = 0;
    uint32_t gl_element_buffer_object = 0;

    uint32_t attributes = 0;
    GLenum primitive_mode = 0;
    uint32_t element_count = 0;
};

struct PrimitiveData {
    PrimitiveData() = default;
    PrimitiveData(const PrimitiveData&) = default;
    PrimitiveData& operator=(PrimitiveData&&);

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> tex_coords;
    std::vector<float> colors;
    std::vector<uint8_t> joint_indices;

    std::vector<uint16_t> indices_array_uint16;
    std::vector<uint32_t> indices_array_uint32;

    GLMeshInfo createGlMesh();

    Box computeBoundingBox() const;
};



}
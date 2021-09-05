#pragma once

#include "framework64/mesh.h"
#include "framework64/desktop/material.h"

#include <gl/glew.h>

#include <array>
#include <string>
#include <vector>

struct fw64Mesh {
    struct LoadOptions {
        bool bakeTransform = false;
    };

    struct Primitive {
        enum Mode { Triangles = GL_TRIANGLES, Lines = GL_LINES, Unknown = 0};
        enum Attributes { Positions = 1, Normals = 2, TexCoords = 4, VertexColors = 8 };

        framework64::Material material;
        Mode mode;
        uint32_t attributes;
        Box bounding_box;

        uint32_t gl_vertex_array_object;
        uint32_t gl_array_buffer_object;
        uint32_t gl_element_buffer_object;
        GLsizei element_count;
        GLenum element_type;
    };

    fw64Mesh();

    Box bounding_box;

    std::vector<Primitive> primitives;
};
#pragma once

#include "framework64/mesh.h"

#include "libdragon_material_bundle.h"

#include <GL/gl.h>

struct fw64Primitive {
    GLenum mode;
    GLfloat* vertex_data;
    GLuint vertex_count;
    GLuint vertex_stride;
    uint32_t vertex_attributes;

    GLushort* element_data;
    GLuint element_count;
};

struct fw64Mesh {
    fw64Primitive* primitives;
    uint16_t primitive_count;
    fw64MaterialBundle* material_bundle;
    fw64MaterialCollection material_collection;
    Box bounding_box;
};
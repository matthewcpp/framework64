#pragma once

#include "framework64/n64_libdragon/material_bundle.h"

#include "framework64/allocator.h"
#include "framework64/box.h"
#include "framework64/data_io.h"
#include "framework64/mesh.h"

#include <GL/gl.h>
#include <stdint.h>

// note this should be kept in sync with pipeline/desktop/MeshWriter.js
typedef enum { 
    FW64_PRIMITIVE_ATTRIBUTES_POSITIONS = 1,
    FW64_PRIMITIVE_ATTRIBUTES_NORMALS = 2,
    FW64_PRIMITIVE_ATTRIBUTES_TEXCOORDS = 4,
    FW64_PRIMITIVE_ATTRIBUTES_COLORS = 8
} fw64GLPrimitiveAttributes;

typedef enum { 
    FW64_GLPRIMITIVE_MODE_UNKNOWN = 0,
    FW64_GLPRIMITIVE_MODE_TRIANGLES = GL_TRIANGLES, 
    FW64_GLPRIMITIVE_MODE_LINES = GL_LINES 
} fw64GLPrimitiveMode;

/// Note this should coorespond to pipeline/GLMeshWriter.js _writePimitiveInfo
typedef struct {
    uint32_t vertex_count;
    uint32_t vertex_attributes;
    uint32_t element_count;
    uint32_t mode;
    uint32_t material_index;
    uint32_t joint_index;
    Box bounding_box;
} fw64PrimitiveInfo;

typedef struct {
    float* positions;
    float* normals;
    float* tex_coords;
    uint8_t* colors;
    uint16_t* elements;
} fw64PrimitiveData;

struct fw64Primitive {
    fw64PrimitiveInfo info;
    fw64PrimitiveData data;

    // TODO: encapsulate this better for future platform re-use?
    uint32_t gl_vertex_array_object;
    uint32_t gl_array_buffer_object;
    uint32_t gl_element_buffer_object;
};

/// Note this should coorespond to pipeline/desktop/MeshWriter.js _writeMeshInfo
typedef struct {
    uint32_t primitive_count;
    fw64MaterialBundle* material_bundle;
    Box bounding_box;
} fw64MeshInfo;

struct fw64Mesh{
    fw64MeshInfo info;
    fw64Primitive* primitives;
    Box bounding_box;
};

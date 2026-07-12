#pragma once

#include "framework64/mesh.h"

#include "libdragon_material_bundle.h"

#include <GL/gl.h>

typedef struct {
    float position[3];
    uint32_t color;
} fw64UnlitVert;

typedef struct {
    float position[3];
    float texcoord[2];
    uint32_t color;
} fw64UnlitTexturedVert;

typedef struct {
    float position[3];
    float normal[3];
} fw64LitVert;

typedef struct {
    float position[3];
    float normal[3];
    float texcoord[2];
} fw64LitTexturedVert;


typedef struct {
    uint32_t vertex_count;
    uint32_t vertex_data_size;
    uint32_t vertex_attributes;
    uint32_t mode;
    uint32_t element_count;
    uint32_t joint_index;
} PrimitiveInfo;

struct fw64Primitive {
    PrimitiveInfo info;
    GLfloat* vertex_data;
    GLushort* element_data;
};

struct fw64Mesh {
    fw64Primitive* primitives;
    uint32_t primitive_count;
    fw64MaterialBundle* material_bundle;
    fw64MaterialCollection material_collection;
    Box bounding_box;
};
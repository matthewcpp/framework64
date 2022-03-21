#pragma once

#include "framework64/asset_database.h"
#include "framework64/mesh.h"
#include "framework64/desktop/material.h"
#include "framework64/desktop/texture.h"

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace framework64 {

struct SharedResources {
    std::vector<std::unique_ptr<fw64Texture>> textures;
    std::vector<std::unique_ptr<fw64Image>> images;
};

}

struct fw64Primitive {
    enum Mode { Triangles = GL_TRIANGLES, Lines = GL_LINES, Unknown = 0};
    enum Attributes { Positions = 1, Normals = 2, TexCoords = 4, VertexColors = 8 };

    fw64Material material;
    Mode mode = Mode::Unknown;
    uint32_t attributes = 0;
    uint32_t joint_index = FW64_JOINT_INDEX_NONE;
    Box bounding_box;

    GLuint  gl_vertex_array_object = 0;
    GLuint  gl_array_buffer_object = 0;
    GLuint  gl_element_buffer_object = 0;
    GLsizei element_count = 0;
    GLenum element_type = 0;

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> tex_coords;
    std::vector<float> colors;
    std::vector<uint16_t> indices;
};

struct fw64Mesh {
    fw64Mesh();
    ~fw64Mesh();

    Box bounding_box;

    std::vector<fw64Primitive> primitives;
    std::unique_ptr<framework64::SharedResources> resources;

    static fw64Mesh* loadFromDatabase(fw64AssetDatabase* database, uint32_t index);
};
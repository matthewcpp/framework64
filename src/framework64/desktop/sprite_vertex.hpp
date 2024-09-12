#pragma once

#include "framework64/desktop/texture.hpp"

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <array>
#include <vector>

namespace framework64 {

struct SpriteVertex {
    float x, y, z;
    float u, v;
    float r, g, b, a;
};

struct SpriteVertexBuffer {
    void init();
    void deleteGlObjects();
    void updateBufferData();
    void addSpriteFrame(fw64Texture* texture, int frame, float x, float y, float width, float height, const std::array<float, 4>& fill_color);
    void addQuad(SpriteVertex const & a, SpriteVertex const & b, SpriteVertex const & c, SpriteVertex const & d);
    void createQuad(float xf, float yf, float wf, float hf, std::array<float, 4> const & color);

    void clear();

    GLuint gl_vertex_array_object = 0;
    GLuint gl_vertex_buffer = 0;
    size_t gl_vertex_buffer_size_in_bytes = 0;
    std::vector<SpriteVertex> cpu_buffer;
};

}
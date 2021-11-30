#pragma once

#include "framework64/camera.h"

#include "framework64/desktop/font.h"
#include "framework64/desktop/shader_cache.h"
#include "framework64/desktop/texture.h"

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <array>
#include <string>
#include <vector>

namespace framework64 {

class SpriteRenderer {
public:
    SpriteRenderer() = default;

    bool init(ShaderCache& shader_cache);
    void setScreenSize(int width, int height);
    void begin(fw64Camera const * camera);
    void end();

    void drawSprite(fw64Texture const * texture, float x, float y);
    void drawSpriteFrame(fw64Texture const * texture, int frame, float x, float y);
    void drawText(fw64Font const* font, float x, float y, const char* text, uint32_t count);

private:
    struct SpriteVertex {
        float x, y, z;
        float u, v;
    };

private:
    void setCurrentTexture(fw64Texture const * texture);
    void submitCurrentBatch();
    void addQuad(SpriteVertex const & a, SpriteVertex const & b, SpriteVertex const & c, SpriteVertex const & d);

private:
    GLuint shader;
    GLint uniform_matrix;
    GLint uniform_sampler;

    fw64Texture const * current_texture;

    GLuint gl_vertex_array_object;
    GLuint gl_vertex_buffer;
    size_t gl_vertex_buffer_size_in_bytes = 0;
    std::vector<SpriteVertex> vertex_buffer;
    std::array<float, 16> matrix;

};

}


#pragma once

#include "framework64/camera.h"

#include "framework64/desktop/texture.h"

#include <gl/glew.h>

#include <array>
#include <string>
#include <vector>

namespace framework64 {

class SpriteRenderer {
public:
    SpriteRenderer() = default;

    bool init(std::string const & shader_dir);
    void setScreenSize(int width, int height);
    void begin(fw64Camera* camera);
    void end();

    void drawSprite(fw64Texture* texture, int x, int y);
    void drawSpriteFrame(fw64Texture* texture, int frame, int x, int y);

private:
    void submitCurrentBatch();

private:
    struct SpriteVertex {
        float x, y, z;
        float u, v;
    };

private:
    GLuint shader;
    GLint uniform_matrix;
    GLint uniform_sampler;

    fw64Texture* current_texture;

    GLuint gl_vertex_array_object;
    GLuint gl_vertex_buffer;
    size_t gl_vertex_buffer_size_in_bytes = 0;
    std::vector<SpriteVertex> vertex_buffer;
    std::array<float, 16> matrix;

};

}


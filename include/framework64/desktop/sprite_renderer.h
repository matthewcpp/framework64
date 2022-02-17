#pragma once

#include "framework64/camera.h"

#include "framework64/desktop/font.h"
#include "framework64/desktop/material.h"
#include "framework64/desktop/pixel_texture.h"
#include "framework64/desktop/shader_cache.h"
#include "framework64/desktop/texture.h"
#include "framework64/desktop/uniform_block.h"

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
    
    void start();
    void flush();

    void drawSprite(fw64Texture* texture, float x, float y);
    void drawSpriteFrame(fw64Texture* texture, int frame, float x, float y, float scale_x, float scale_y);
    void drawText(fw64Font* font, float x, float y, const char* text, uint32_t count);

    void drawPixelTexture(PixelTexture& pixel_texture);

private:
    void drawSpriteVertices(SpriteVertex const * vertices, size_t vertex_count, fw64Material& material);
    void setCurrentTexture(fw64Texture* texture);
    void submitCurrentBatch();
    void addQuad(SpriteVertex const & a, SpriteVertex const & b, SpriteVertex const & c, SpriteVertex const & d);

private:
    GLuint gl_vertex_array_object = 0;
    GLuint gl_vertex_buffer = 0;
    size_t gl_vertex_buffer_size_in_bytes = 0;
    std::vector<SpriteVertex> vertex_buffer;

    fw64Material sprite_material;

    struct SpriteTransformData{
        std::array<float, 16> mvp_matrix;
    };

    UniformBlock<SpriteTransformData> sprite_transform_uniform_block;

};

}


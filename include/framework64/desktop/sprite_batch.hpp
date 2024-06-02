#pragma once

#include "framework64/camera.h"
#include "framework64/sprite_batch.h"

#include "framework64/desktop/font.hpp"
#include "framework64/desktop/material.hpp"
#include "framework64/desktop/shader_cache.hpp"
#include "framework64/desktop/sprite_vertex.hpp"
#include "framework64/desktop/texture.hpp"
#include "framework64/desktop/uniform_block.hpp"

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace framework64 {

class SpriteBatch {
public:
    SpriteBatch() = default;

    bool init(ShaderCache& shader_cache);
    void setScreenSize(int width, int height);
    
    void start();
    void flush();

    void drawSprite(fw64Texture* texture, float x, float y);
    void drawSpriteFrame(fw64Texture* texture, int frame, float x, float y, float scale_x, float scale_y);
    void drawText(fw64Font* font, float x, float y, const char* text, uint32_t count);
    void drawSpriteBatch(fw64SpriteBatch* sprite_batch);

    void setFillColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void drawFilledRect(float x, float y, float width, float height);

    void newFrame();

private:
    void drawSpriteVertices(fw64Material& material);
    void drawSpriteVertices(SpriteVertexBuffer& sprite_vertex_buffer, fw64Material& material);
    void setCurrentTexture(fw64Texture* texture);
    void submitCurrentBatch();
    void createQuad(float xf, float yf, float wf, float hf, std::array<float, 4> const & color);

private:
    SpriteVertexBuffer vertex_buffer;

    fw64Material sprite_material;
    fw64Material fill_material;
    fw64Material* current_material = nullptr;
    GLuint current_shader_handle;

    struct SpriteTransformData{
        std::array<float, 16> mvp_matrix;
    };

    UniformBlock<SpriteTransformData> sprite_transform_uniform_block;
    std::array<float, 4> fill_color = {1.0f, 1.0f, 1.0f, 1.0f};
};

}


struct fw64SpriteBatch {
    fw64SpriteBatch(fw64Allocator* allocator, size_t num_layers);
    ~fw64SpriteBatch();

    void begin();
    void end();

    void addSprite(fw64Texture* texture, int x, int y);
    void addSpriteSlice(fw64Texture* texture, int frame, float x, float y);
    void addSpriteSlice(fw64Texture* texture, int frame, float x, float y, float width, float height);
    void addText(fw64Font* font, float x, float y, const char* text, uint32_t count);

    framework64::SpriteVertexBuffer& getBuffer(fw64Texture* texture);

    fw64Allocator* allocator;

    std::vector<framework64::SpriteVertexBuffer> vertex_buffers;
    size_t next_buffer_index;

    struct Layer {
        std::unordered_map<fw64Texture*, size_t> batches;
    };

    std::vector<Layer> layers;
    Layer* active_layer;
    
    std::array<float, 4> fill_color = {1.0f, 1.0f, 1.0f, 1.0f};
    bool is_finalized = false;
};

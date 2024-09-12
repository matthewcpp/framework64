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

struct fw64SpriteBatch {
    fw64SpriteBatch(fw64Allocator* allocator, size_t num_layers);
    ~fw64SpriteBatch();

    void begin();
    void end();

    void addSprite(fw64Texture* texture, float x, float y);
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

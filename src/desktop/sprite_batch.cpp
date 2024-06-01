#include "framework64/desktop/sprite_batch.hpp"

#include "framework64/matrix.h"

#include "framework64/desktop/shader.hpp"

namespace framework64 {
    bool SpriteBatch::init(ShaderCache& shader_cache) {
        vertex_buffer.init();

        sprite_material.texture_frame = FW64_DESKTOP_ENTIRE_TEXTURE_FRAME;
        sprite_material.shader = shader_cache.getShaderProgram(FW64_SHADING_MODE_SPRITE);
        sprite_material.shading_mode = FW64_SHADING_MODE_SPRITE;

        fill_material.texture_frame = FW64_DESKTOP_ENTIRE_TEXTURE_FRAME;
        fill_material.shader = shader_cache.getShaderProgram(FW64_SHADING_MODE_FILL);
        fill_material.shading_mode = FW64_SHADING_MODE_FILL;

        sprite_transform_uniform_block.create();

        return true;
    }

    void SpriteBatch::start() {
        glDisable(GL_DEPTH_TEST);
        sprite_material.texture = nullptr;
        sprite_material.texture_frame = FW64_DESKTOP_ENTIRE_TEXTURE_FRAME;
        current_shader_handle = 0;
    }

    void SpriteBatch::newFrame() {
        std::fill(fill_color.begin(), fill_color.end(), 1.0f);
    }

    void SpriteBatch::flush() {
        submitCurrentBatch();
    }

    void SpriteBatch::submitCurrentBatch() {
        if (vertex_buffer.cpu_buffer.size() == 0 || current_material == nullptr)
            return;

        drawSpriteVertices(*current_material);
        vertex_buffer.cpu_buffer.clear();
        current_material->texture = nullptr;
        current_material->texture_frame = FW64_DESKTOP_ENTIRE_TEXTURE_FRAME;
        current_material = nullptr;
    }

    void SpriteBatch::drawSpriteVertices(fw64Material& material) {
        if (vertex_buffer.cpu_buffer.empty())
            return;

        vertex_buffer.updateBufferData();

        drawSpriteVertices(vertex_buffer, material);
    }

    void SpriteBatch::drawSpriteVertices(SpriteVertexBuffer& sprite_vertex_buffer, fw64Material& material) {
        auto* current_shader = current_material->shader;
        if (current_shader->handle != current_shader_handle) {
            // activate the specific shader and set the matrix
            glUseProgram(current_shader->handle);
            glUniformBlockBinding(current_shader->handle, current_shader->mesh_transform_uniform_block_index, sprite_transform_uniform_block.binding_index);
            current_shader_handle = current_shader->handle;
        }

        material.shader->shader->setUniforms(material.shader, material);

        glBindVertexArray(sprite_vertex_buffer.gl_vertex_array_object);
        glDrawArrays(GL_TRIANGLES, 0, sprite_vertex_buffer.cpu_buffer.size());
        glBindVertexArray(0);
    }

    void SpriteBatch::setCurrentTexture(fw64Texture* texture) {
        if (!current_material || current_material->texture != texture) {
            submitCurrentBatch();
            current_material = &sprite_material;
            current_material->texture = texture;
            return;
        }        
    }

    void SpriteBatch::drawSpriteBatch(fw64SpriteBatch* sprite_batch) {
        for (const auto & layer : sprite_batch->layers) {
            for (const auto & batch : layer.batches) {
                setCurrentTexture(batch.first);
                drawSpriteVertices(sprite_batch->vertex_buffers[batch.second], sprite_material);
            }
        }
    }

    void SpriteBatch::createQuad(float xf, float yf, float wf, float hf, std::array<float, 4> const & color) {
        SpriteVertex a = {xf, yf, 0.0f, 0.0, 0.0, color[0], color[1], color[2], color[3]};
        SpriteVertex b = {xf + wf, yf, 0.0f, 1.0f, 0.0f, color[0], color[1], color[2], color[3]};
        SpriteVertex c = {xf + wf, yf + hf, 0.0f, 1.0f, 1.0f, color[0], color[1], color[2], color[3]};
        SpriteVertex d = {xf, yf + hf, 0.0f, 0.0f, 1.0f, color[0], color[1], color[2], color[3]};

        vertex_buffer.addQuad(a, b, c, d);
    }

    void SpriteBatch::drawSprite(fw64Texture* texture, float x, float y) {
        setCurrentTexture(texture);
        createQuad(x, y, static_cast<float>(texture->image->width), static_cast<float>(texture->image->height), fill_color);
    }

    void SpriteBatch::drawFilledRect(float x, float y, float width, float height) {
        if (current_material == &sprite_material) {
            submitCurrentBatch();
        }
        if (!current_material) {
            current_material = &fill_material;
        }

        createQuad(x, y, width, height, fill_color);
    }

    void SpriteBatch::drawSpriteFrame(fw64Texture* texture, int frame, float x, float y, float scale_x, float scale_y) {
        setCurrentTexture(texture);

        auto frame_width = static_cast<float>(texture->slice_width() * scale_x);
        auto frame_height = static_cast<float>(texture->slice_height() * scale_y);

        vertex_buffer.addSpriteFrame(texture, frame, x, y, frame_width, frame_height, fill_color);
    }

    void SpriteBatch::drawText(fw64Font* font, float x, float y, const char* text, uint32_t count){
        if (!text || text[0] == 0){
            return;
        }

        uint32_t glyph_index;
        text = font->getNextGlyphIndex(text, glyph_index);
        auto const & first_glyph = font->glyphs[glyph_index];
        x += -first_glyph.left;

        for (uint32_t i = 0; i < count; i++) {
            auto const & glyph = font->glyphs[glyph_index];
            drawSpriteFrame(font->texture.get(), glyph_index, x + glyph.left, y + glyph.top, 1.0f, 1.0f);
            x += glyph.advance;

            if (text[0] == 0) {
                break;
            }

            text = font->getNextGlyphIndex(text, glyph_index);
        }
    }

    void SpriteBatch::setScreenSize(int width, int height) {
        matrix_ortho(sprite_transform_uniform_block.data.mvp_matrix.data(), 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
        sprite_transform_uniform_block.update();
    }

    void SpriteBatch::setFillColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        // compute pre-multiplied alpha
        fill_color[3] = static_cast<float>(a) / 255.0f;

        fill_color[0] = (static_cast<float>(r) / 255.0f) * fill_color[3];
        fill_color[1] = (static_cast<float>(g) / 255.0f) * fill_color[3];
        fill_color[2] = (static_cast<float>(b) / 255.0f) * fill_color[3];
    }
}

// ------------------
#include "framework64/desktop/sprite_batch.hpp"
#include "framework64/desktop/font.hpp"
#include "framework64/desktop/texture.hpp"

#include <cassert>
#include <limits>

fw64SpriteBatch::fw64SpriteBatch(fw64Allocator* allocator, size_t num_layers)
    : allocator(allocator)
{
    assert(allocator != nullptr);

    layers.resize(num_layers);
    active_layer = &layers[0];

    begin();
}

fw64SpriteBatch::~fw64SpriteBatch() {
    for (auto & vertex_buffer: vertex_buffers) {
        vertex_buffer.deleteGlObjects();
    }
}

void fw64SpriteBatch::begin() {
    for (auto & vertex_buffer: vertex_buffers) {
        vertex_buffer.clear();
    }

    for (auto& layer : layers) {
        layer.batches.clear();
    }

    next_buffer_index = 0;
    is_finalized = false;
}

void fw64SpriteBatch::end() {
    for (size_t i = 0; i < next_buffer_index; i++) {
        vertex_buffers[i].updateBufferData();
    }

    is_finalized = true;
}

framework64::SpriteVertexBuffer& fw64SpriteBatch::getBuffer(fw64Texture* texture) {
    auto result = active_layer->batches.find(texture);

    if (result != active_layer->batches.end()) {
        return vertex_buffers[result->second];
    }

    size_t buffer_index = next_buffer_index++;
    active_layer->batches[texture] = buffer_index;

    if (buffer_index >= vertex_buffers.size()) {
        auto& vertex_buffer = vertex_buffers.emplace_back();
        vertex_buffer.init();
    }

    return vertex_buffers[buffer_index];
}

void fw64SpriteBatch::addSprite(fw64Texture* texture, int x, int y) {
    auto& vertex_buffer = getBuffer(texture);
    vertex_buffer.createQuad(x, y, static_cast<float>(texture->image->width), static_cast<float>(texture->image->height), fill_color);
}

void fw64SpriteBatch::addSpriteSlice(fw64Texture* texture, int frame, float x, float y) {
    auto& vertex_buffer = getBuffer(texture);
    auto slice_width = static_cast<float>(texture->slice_width());
    auto slice_height = static_cast<float>(texture->slice_height());

    vertex_buffer.addSpriteFrame(texture, frame, x, y, slice_width, slice_height, fill_color);
}

void fw64SpriteBatch::addSpriteSlice(fw64Texture* texture, int frame, float x, float y, float width, float height) {
    auto& vertex_buffer = getBuffer(texture);
    vertex_buffer.addSpriteFrame(texture, frame, x, y, width, height, fill_color);
}

void fw64SpriteBatch::addText(fw64Font* font, float x, float y, const char* text, uint32_t count) {
    if (!text || text[0] == 0){
        return;
    }

    auto slice_width = static_cast<float>(font->texture->slice_width());
    auto slice_height = static_cast<float>(font->texture->slice_height());
    auto& vertex_buffer = getBuffer(font->texture.get());

    uint32_t glyph_index;
    text = font->getNextGlyphIndex(text, glyph_index);
    auto const & first_glyph = font->glyphs[glyph_index];
    x += -first_glyph.left;

    for (uint32_t i = 0; i < count; i++) {
        auto const & glyph = font->glyphs[glyph_index];
        vertex_buffer.addSpriteFrame(font->texture.get(), glyph_index, x + glyph.left, y + glyph.top, slice_width, slice_height, fill_color);
        x += glyph.advance;

        if (text[0] == 0) {
            break;
        }

        text = font->getNextGlyphIndex(text, glyph_index);
    }
}

// C API

fw64SpriteBatch* fw64_spritebatch_create(size_t num_layers, fw64Allocator* allocator) {
    assert(allocator);
    assert(num_layers > 0);

    return new fw64SpriteBatch(allocator, num_layers);
}

void fw64_spritebatch_set_color(fw64SpriteBatch* sprite_batch, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    sprite_batch->fill_color[0] = static_cast<float>(r) / 255.0f;
    sprite_batch->fill_color[1] = static_cast<float>(g) / 255.0f;
    sprite_batch->fill_color[2] = static_cast<float>(b) / 255.0f;
    sprite_batch->fill_color[3] = static_cast<float>(a) / 255.0f;
}

void fw64_spritebatch_begin(fw64SpriteBatch* sprite_batch) {
    sprite_batch->begin();
}

void fw64_spritebatch_delete(fw64SpriteBatch* sprite_batch) {
    delete sprite_batch;
}

void fw64_spritebatch_end(fw64SpriteBatch* sprite_batch) {
    sprite_batch->end();
}

size_t fw64_spritebatch_get_layer_count(fw64SpriteBatch* sprite_batch){
    return sprite_batch->layers.size();
}

int fw64_spritebatch_set_active_layer(fw64SpriteBatch* sprite_batch, size_t layer_index) {
    if (layer_index >= sprite_batch->layers.size()) {
        return 0;
    }

    sprite_batch->active_layer = &sprite_batch->layers[layer_index];

    return 1;
}

void fw64_spritebatch_draw_sprite(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int x, int y) {
    sprite_batch->addSprite(texture, x, y);
}

void fw64_spritebatch_draw_sprite_slice(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int frame, int x, int y) {
    sprite_batch->addSpriteSlice(texture, frame, x, y);
}

void fw64_spritebatch_draw_sprite_slice_rect(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int frame, int x, int y, int width, int height) {
    sprite_batch->addSpriteSlice(texture, frame, x, y, width, height);
}

void fw64_spritebatch_draw_string(fw64SpriteBatch* sprite_batch, fw64Font* font, const char* text, int x, int y) {
    sprite_batch->addText(font, x, y, text, std::numeric_limits<uint32_t>::max());
}

void fw64_spritebatch_draw_string_count(fw64SpriteBatch* sprite_batch, fw64Font* font, const char* text, uint32_t count, int x, int y) {
    sprite_batch->addText(font, x, y, text, count);
}

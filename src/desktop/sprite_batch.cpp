#include "framework64/desktop/sprite_batch.hpp"

#include "framework64/matrix.h"

#include "framework64/desktop/shader.hpp"

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

void fw64SpriteBatch::addSprite(fw64Texture* texture, float x, float y) {
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
    sprite_batch->addSprite(texture, static_cast<float>(x), static_cast<float>(y));
}

void fw64_spritebatch_draw_sprite_slice(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int frame, int x, int y) {
    sprite_batch->addSpriteSlice(texture, frame, static_cast<float>(x), static_cast<float>(y));
}

void fw64_spritebatch_draw_sprite_slice_rect(fw64SpriteBatch* sprite_batch, fw64Texture* texture, int frame, int x, int y, int width, int height) {
    sprite_batch->addSpriteSlice(texture, frame, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
}

void fw64_spritebatch_draw_string(fw64SpriteBatch* sprite_batch, fw64Font* font, const char* text, int x, int y) {
    sprite_batch->addText(font, static_cast<float>(x), static_cast<float>(y), text, std::numeric_limits<uint32_t>::max());
}

void fw64_spritebatch_draw_string_count(fw64SpriteBatch* sprite_batch, fw64Font* font, const char* text, uint32_t count, int x, int y) {
    sprite_batch->addText(font, static_cast<float>(x), static_cast<float>(y), text, count);
}

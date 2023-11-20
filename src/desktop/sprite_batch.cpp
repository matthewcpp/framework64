#include "framework64/desktop/sprite_batch.hpp"

#include "framework64/matrix.h"

#include "framework64/desktop/shader.hpp"

namespace framework64 {
    bool SpriteBatch::init(ShaderCache& shader_cache) {
        glGenBuffers(1, &gl_vertex_buffer);

        glGenVertexArrays(1, &gl_vertex_array_object);
        glBindVertexArray(gl_vertex_array_object);

        glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer);

        // enable positions in VAO
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)0);

        // enable tex coords in VAO
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)12);

        // enable vertex colors in VAO
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)20);

        glBindVertexArray(0);

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
        if (vertex_buffer.size() == 0 || current_material == nullptr)
            return;

        drawSpriteVertices(vertex_buffer.data(), vertex_buffer.size(), *current_material);
        vertex_buffer.clear();
        current_material->texture = nullptr;
        current_material->texture_frame = FW64_DESKTOP_ENTIRE_TEXTURE_FRAME;
        current_material = nullptr;
    }

    void SpriteBatch::drawSpriteVertices(SpriteVertex const* vertices, size_t vertex_count, fw64Material& material) {
        if (vertices == nullptr || vertex_count == 0)
            return;
        // upload all vertices to GPU
        glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer);

        size_t vertex_buffer_size_in_bytes = vertex_count * sizeof(SpriteVertex);

        if (vertex_buffer_size_in_bytes > gl_vertex_buffer_size_in_bytes) {
            glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size_in_bytes, vertices, GL_DYNAMIC_DRAW);
            gl_vertex_buffer_size_in_bytes = vertex_buffer_size_in_bytes;
        }
        else {
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_buffer_size_in_bytes, vertices);
        }

        auto* current_shader = current_material->shader;
        if (current_shader->handle != current_shader_handle) {
            // activate the specific shader and set the matrix
            glUseProgram(current_shader->handle);
            glUniformBlockBinding(current_shader->handle, current_shader->mesh_transform_uniform_block_index, sprite_transform_uniform_block.binding_index);
            current_shader_handle = current_shader->handle;
        }

        material.shader->shader->setUniforms(material.shader, material);

        glBindVertexArray(gl_vertex_array_object);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
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

    void SpriteBatch::createQuad(float xf, float yf, float wf, float hf, std::array<float, 4> const & color) {
        SpriteVertex a = {xf, yf, 0.0f, 0.0, 0.0, color[0], color[1], color[2], color[3]};
        SpriteVertex b = {xf + wf, yf, 0.0f, 1.0f, 0.0f, color[0], color[1], color[2], color[3]};
        SpriteVertex c = {xf + wf, yf + hf, 0.0f, 1.0f, 1.0f, color[0], color[1], color[2], color[3]};
        SpriteVertex d = {xf, yf + hf, 0.0f, 0.0f, 1.0f, color[0], color[1], color[2], color[3]};

        addQuad(a, b, c, d);
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

        auto wf = static_cast<float>(texture->slice_width());
        auto hf = static_cast<float>(texture->slice_height());

        // calculate the texcord coordinate window
        float tc_width = wf / static_cast<float>(texture->image->width);
        float tc_height = hf / static_cast<float>(texture->image->height);
        float tc_x = static_cast<float>(frame % texture->image->hslices) * tc_width;
        float tc_y = 1.0f - static_cast<float>(frame / texture->image->hslices) * tc_height;

        wf *= scale_x;
        hf *= scale_y;

        // note y tex-coord is flipped based on SDL loading
        SpriteVertex a = {x, y, 0.0f, tc_x, 1.0f - tc_y, fill_color[0], fill_color[1], fill_color[2], fill_color[3]};
        SpriteVertex b = {x + wf, y, 0.0f, tc_x + tc_width, 1.0f - tc_y, fill_color[0], fill_color[1], fill_color[2], fill_color[3]};
        SpriteVertex c = {x + wf, y + hf, 0.0f, tc_x + tc_width, 1.0f - (tc_y - tc_height), fill_color[0], fill_color[1], fill_color[2], fill_color[3]};
        SpriteVertex d = {x, y + hf, 0.0f, tc_x, 1.0f - (tc_y - tc_height), fill_color[0], fill_color[1], fill_color[2], fill_color[3]};

        addQuad(a, b, c, d);
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

    void SpriteBatch::addQuad(SpriteVertex const & a, SpriteVertex const & b, SpriteVertex const & c, SpriteVertex const & d) {
        vertex_buffer.push_back(a);
        vertex_buffer.push_back(b);
        vertex_buffer.push_back(c);

        vertex_buffer.push_back(a);
        vertex_buffer.push_back(c);
        vertex_buffer.push_back(d);
    }

    void SpriteBatch::setFillColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        fill_color[0] = static_cast<float>(r) / 255.0f;
        fill_color[1] = static_cast<float>(g) / 255.0f;
        fill_color[2] = static_cast<float>(b) / 255.0f;
        fill_color[3] = static_cast<float>(a) / 255.0f;
    }
}

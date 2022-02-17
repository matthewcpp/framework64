#include "framework64/desktop/sprite_renderer.h"

#include "framework64/matrix.h"

#include "framework64/desktop/shader.h"

namespace framework64 {
    bool SpriteRenderer::init(ShaderCache& shader_cache) {
        glGenBuffers(1, &gl_vertex_buffer);

        glGenVertexArrays(1, &gl_vertex_array_object);
        glBindVertexArray(gl_vertex_array_object);

        // enable positions in VAO
        glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)0);

        // enable tex coords in VAO
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)12);

        glBindVertexArray(0);

        sprite_material.texture_frame = FW64_DESKTOP_ENTIRE_TEXTURE_FRAME;
        sprite_material.shader = shader_cache.getSpriteShaderProgram();
        sprite_transform_uniform_block.create();

        return true;
    }

    void SpriteRenderer::start() {
        glDisable(GL_DEPTH_TEST);
        sprite_material.texture = nullptr;

        auto* sprite_shader = sprite_material.shader;

        // activate the sprite shader and set the matrix
        glUseProgram(sprite_shader->handle);
        glUniformBlockBinding(sprite_material.shader->handle, sprite_shader->mesh_transform_uniform_block_index, sprite_transform_uniform_block.binding_index);
    }

    void SpriteRenderer::flush() {
        submitCurrentBatch();
    }

    void SpriteRenderer::drawPixelTexture(PixelTexture& pixel_texture) {
        glDisable(GL_DEPTH_TEST);

        auto* sprite_shader = sprite_material.shader;
        glUseProgram(sprite_shader->handle);

        matrix_set_identity(sprite_transform_uniform_block.data.mvp_matrix.data());
        glUniformBlockBinding(sprite_material.shader->handle, sprite_shader->mesh_transform_uniform_block_index, sprite_transform_uniform_block.binding_index);
        sprite_transform_uniform_block.update();

        pixel_texture.material.shader->shader->setUniforms(pixel_texture.material);
        drawSpriteVertices(pixel_texture.sprite_vertices.data(), pixel_texture.sprite_vertices.size(), pixel_texture.material);
    }

    void SpriteRenderer::submitCurrentBatch() {
        if (vertex_buffer.size() == 0)
            return;

        drawSpriteVertices(vertex_buffer.data(), vertex_buffer.size(), sprite_material);
        vertex_buffer.clear();
    }

    void SpriteRenderer::drawSpriteVertices(SpriteVertex const* vertices, size_t vertex_count, fw64Material& material) {
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

        material.shader->shader->setUniforms(material);

        glBindVertexArray(gl_vertex_array_object);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
        glBindVertexArray(0);
    }

    void SpriteRenderer::setCurrentTexture(fw64Texture* texture) {
        if (texture != sprite_material.texture) {
            if (sprite_material.texture != nullptr)
                submitCurrentBatch();

            sprite_material.texture = texture;
        }
    }

    void SpriteRenderer::drawSprite(fw64Texture* texture, float x, float y) {
        setCurrentTexture(texture);

        auto xf = static_cast<float>(x);
        auto yf = static_cast<float>(y);
        auto wf = static_cast<float>(texture->image->width);
        auto hf = static_cast<float>(texture->image->height);

        SpriteVertex a = {xf, yf, 0.0f, 0.0, 0.0};
        SpriteVertex b = {xf + wf, yf, 0.0f, 1.0f, 0.0f};
        SpriteVertex c = {xf + wf, yf + hf, 0.0f, 1.0f, 1.0f};
        SpriteVertex d = {xf, yf + hf, 0.0f, 0.0f, 1.0f};

        addQuad(a, b, c, d);
    }

    void SpriteRenderer::drawSpriteFrame(fw64Texture* texture, int frame, float x, float y, float scale_x, float scale_y) {
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
        SpriteVertex a = {x, y, 0.0f, tc_x, 1.0f - tc_y};
        SpriteVertex b = {x + wf, y, 0.0f, tc_x + tc_width, 1.0f - tc_y};
        SpriteVertex c = {x + wf, y + hf, 0.0f, tc_x + tc_width, 1.0f - (tc_y - tc_height)};
        SpriteVertex d = {x, y + hf, 0.0f, tc_x, 1.0f - (tc_y - tc_height)};

        addQuad(a, b, c, d);
    }

    void SpriteRenderer::drawText(fw64Font* font, float x, float y, const char* text, uint32_t count){
        if (!text || text[0] == 0) return;

        uint32_t glyph_index;
        text = font->getNextGlyphIndex(text, glyph_index);
        auto const & first_glyph = font->glyphs[glyph_index];
        x += -first_glyph.left;

        for (uint32_t i = 0; i < count; i++) {
            auto const & glyph = font->glyphs[glyph_index];
            drawSpriteFrame(font->texture.get(), glyph_index, x + glyph.left, y + glyph.top, 1.0f, 1.0f);
            x += glyph.advance;

            if (text[0] == 0)
                break;

            text = font->getNextGlyphIndex(text, glyph_index);
        }
    }

    void SpriteRenderer::setScreenSize(int width, int height) {
        matrix_ortho(sprite_transform_uniform_block.data.mvp_matrix.data(), 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
        sprite_transform_uniform_block.update();
    }

    void SpriteRenderer::addQuad(SpriteVertex const & a, SpriteVertex const & b, SpriteVertex const & c, SpriteVertex const & d) {
        vertex_buffer.push_back(a);
        vertex_buffer.push_back(b);
        vertex_buffer.push_back(c);

        vertex_buffer.push_back(a);
        vertex_buffer.push_back(c);
        vertex_buffer.push_back(d);
    }
}
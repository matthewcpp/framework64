#include "framework64/desktop/sprite_renderer.h"

#include "framework64/matrix.h"

#include "framework64/desktop/shader.h"

namespace framework64 {
    bool SpriteRenderer::init(ShaderCache& shader_cache) {
        // TODO: create sprite shader object
        std::string shader_dir = shader_cache.shaderDir();

        // compile sprite shader and setup uniform locations
        const std::string vertex_path = shader_dir + "sprite.vert.glsl";
        const std::string frag_path = shader_dir + "sprite.frag.glsl";

        shader = Shader::createFromPaths(vertex_path, frag_path, {});

        if (!shader) {
            return false;
        }

        uniform_matrix = glGetUniformLocation(shader, "matrix");
        uniform_sampler = glGetUniformLocation(shader, "sampler");

        glGenBuffers(1, &gl_vertex_buffer);

        glGenVertexArrays(1, &gl_vertex_array_object);
        glBindVertexArray(gl_vertex_array_object);

        // enable positions in VAO
        glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)0);

        // enable tex coords in VAO
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)12);

        glBindVertexArray(0);

        return true;
    }

    void SpriteRenderer::begin(fw64Camera const * camera) {
        (void)camera;

        glDisable(GL_DEPTH_TEST);

        glUseProgram(shader);
        glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, matrix.data());
        current_texture = nullptr;
    }

    void SpriteRenderer::end() {
        submitCurrentBatch();
    }

    void SpriteRenderer::drawPixelTexture(PixelTexture const & pixel_texture) {
        // update the texture
        glBindTexture(GL_TEXTURE_2D, pixel_texture.gl_texture_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_texture.pixel_data.data());

        glDisable(GL_DEPTH_TEST);

        glUseProgram(shader);
        glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, pixel_texture.matrix.data());
        drawSpriteVertices(pixel_texture.sprite_vertices.data(), pixel_texture.sprite_vertices.size(), pixel_texture.gl_texture_handle);
    }

    void SpriteRenderer::submitCurrentBatch() {
        if (vertex_buffer.size() == 0)
            return;

        drawSpriteVertices(vertex_buffer.data(), vertex_buffer.size(), current_texture->image->gl_handle);
        vertex_buffer.clear();
    }

    void SpriteRenderer::drawSpriteVertices(SpriteVertex const* vertices, size_t vertex_count, GLuint texture_handle) {
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

        // set the current texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_handle);
        glUniform1i(uniform_sampler, 0);

        glBindVertexArray(gl_vertex_array_object);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
        glBindVertexArray(0);
    }

    void SpriteRenderer::setCurrentTexture(fw64Texture const* texture) {
        if (texture != current_texture) {
            if (current_texture != nullptr)
                submitCurrentBatch();

            current_texture = texture;
        }
    }

    void SpriteRenderer::drawSprite(fw64Texture const * texture, float x, float y) {
        setCurrentTexture(texture);

        auto xf = static_cast<float>(x);
        auto yf = static_cast<float>(y);
        auto wf = static_cast<float>(texture->image->width);
        auto hf = static_cast<float>(texture->image->height);

        SpriteVertex a = {xf, yf, 0.0f, 0.0, 1.0};
        SpriteVertex b = {xf + wf, yf, 0.0f, 1.0f, 1.0f};
        SpriteVertex c = {xf + wf, yf + hf, 0.0f, 1.0f, 0.0f};
        SpriteVertex d = {xf, yf + hf, 0.0f, 0.0f, 0.0f};

        addQuad(a, b, c, d);
    }

    void SpriteRenderer::drawSpriteFrame(fw64Texture const * texture, int frame, float x, float y, float scale_x, float scale_y) {
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

        SpriteVertex a = {x, y, 0.0f, tc_x, tc_y};
        SpriteVertex b = {x + wf, y, 0.0f, tc_x + tc_width, tc_y};
        SpriteVertex c = {x + wf, y + hf, 0.0f, tc_x + tc_width, tc_y - tc_height};
        SpriteVertex d = {x, y + hf, 0.0f, tc_x, tc_y - tc_height};

        addQuad(a, b, c, d);
    }

    void SpriteRenderer::drawText(fw64Font const * font, float x, float y, const char* text, uint32_t count){
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
        screen_width = width;
        screen_height = height;

        matrix_ortho(matrix.data(), 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
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
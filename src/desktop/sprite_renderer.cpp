#include "framework64/desktop/sprite_renderer.h"

#include "framework64/matrix.h"

#include "framework64/desktop/shader.h"

namespace framework64 {
    bool SpriteRenderer::init(std::string const & shader_dir) {
        // compile sprite shader and setup uniform locations
        const std::string vertex_path = shader_dir + "sprite.vert.glsl";
        const std::string frag_path = shader_dir + "sprite.frag.glsl";

        shader = Shader::createFromPaths(vertex_path, frag_path);

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
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), (void*)12);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        return true;
    }

    void SpriteRenderer::begin(fw64Camera* camera) {
        (void)camera;

        glUseProgram(shader);
        glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, matrix.data());
        current_texture = nullptr;
    }

    void SpriteRenderer::end() {
        submitCurrentBatch();
    }

    void SpriteRenderer::submitCurrentBatch() {
        if (vertex_buffer.size() == 0)
            return;

        // upload all vertices to GPU
        glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer);

        size_t vertex_buffer_size_in_bytes = vertex_buffer.size() * sizeof(SpriteVertex);

        if (vertex_buffer_size_in_bytes > gl_vertex_buffer_size_in_bytes) {
            glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size_in_bytes, vertex_buffer.data(), GL_DYNAMIC_DRAW);
            gl_vertex_buffer_size_in_bytes = vertex_buffer_size_in_bytes;
        }
        else {
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_buffer_size_in_bytes, vertex_buffer.data());
        }


        // set the current texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, current_texture->gl_handle);
        glUniform1i(uniform_sampler, 0);

        glBindVertexArray(gl_vertex_array_object);
        glDrawArrays(GL_TRIANGLES, 0, vertex_buffer.size());
        glBindVertexArray(0);

        vertex_buffer.clear();
    }

    void SpriteRenderer::setCurrentTexture(fw64Texture* texture) {
        if (texture != current_texture) {
            if (current_texture != nullptr)
                submitCurrentBatch();

            current_texture = texture;
        }
    }

    void SpriteRenderer::drawSprite(fw64Texture* texture, int x, int y) {
        setCurrentTexture(texture);

        auto xf = static_cast<float>(x);
        auto yf = static_cast<float>(y);
        auto wf = static_cast<float>(texture->width);
        auto hf = static_cast<float>(texture->height);

        SpriteVertex a = {xf, yf, 0.0f, 0.0, 1.0};
        SpriteVertex b = {xf + wf, yf, 0.0f, 1.0f, 1.0f};
        SpriteVertex c = {xf + wf, yf + hf, 0.0f, 1.0f, 0.0f};
        SpriteVertex d = {xf, yf + hf, 0.0f, 0.0f, 0.0f};

        vertex_buffer.push_back(a);
        vertex_buffer.push_back(b);
        vertex_buffer.push_back(c);

        vertex_buffer.push_back(a);
        vertex_buffer.push_back(c);
        vertex_buffer.push_back(d);
    }

    void SpriteRenderer::drawSpriteFrame(fw64Texture* texture, int frame, int x, int y) {
        setCurrentTexture(texture);

        auto xf = static_cast<float>(x);
        auto yf = static_cast<float>(y);
        auto wf = static_cast<float>(fw64_texture_get_slice_width(texture));
        auto hf = static_cast<float>(fw64_texture_get_slice_height(texture));

        float tc_width = wf / texture->width;
        float tc_height = hf / texture->height;
        float tc_x = static_cast<float>(frame % texture->hslices) * tc_width;
        float tc_y = static_cast<float>(frame / texture->hslices) * tc_height;

        SpriteVertex a = {xf, yf, 0.0f, 0.0, 1.0};
        SpriteVertex b = {xf + wf, yf, 0.0f, 1.0f, 1.0f};
        SpriteVertex c = {xf + wf, yf + hf, 0.0f, 1.0f, 0.0f};
        SpriteVertex d = {xf, yf + hf, 0.0f, 0.0f, 0.0f};
    }

    void SpriteRenderer::setScreenSize(int width, int height) {
        matrix_ortho(matrix.data(), 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    }
}
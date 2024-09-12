#include "framework64/desktop/sprite_vertex.hpp"

namespace framework64 {

void SpriteVertexBuffer::init() {
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
}

void SpriteVertexBuffer::deleteGlObjects() {
    glDeleteVertexArrays(1, &gl_vertex_array_object);
    glDeleteBuffers(1, &gl_vertex_buffer);
}

void SpriteVertexBuffer::updateBufferData() {
    // upload all vertices to GPU
    glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer);

    size_t vertex_buffer_size_in_bytes = cpu_buffer.size() * sizeof(SpriteVertex);

    if (vertex_buffer_size_in_bytes > gl_vertex_buffer_size_in_bytes) {
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size_in_bytes, cpu_buffer.data(), GL_DYNAMIC_DRAW);
        gl_vertex_buffer_size_in_bytes = vertex_buffer_size_in_bytes;
    }
    else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_buffer_size_in_bytes, cpu_buffer.data());
    }
}

void SpriteVertexBuffer::addSpriteFrame(fw64Texture* texture, int frame, float x, float y, float wf, float hf, const std::array<float, 4>& fill_color) {
    auto slice_width = static_cast<float>(texture->slice_width());
    auto slice_height = static_cast<float>(texture->slice_height());

    // calculate the texcord coordinate window
    float tc_width = slice_width / static_cast<float>(texture->image->width);
    float tc_height = slice_height / static_cast<float>(texture->image->height);
    float tc_x = static_cast<float>(frame % texture->image->hslices) * tc_width;
    float tc_y = 1.0f - static_cast<float>(frame / texture->image->hslices) * tc_height;

    // note y tex-coord is flipped based on SDL loading
    SpriteVertex a = {x, y, 0.0f, tc_x, 1.0f - tc_y, fill_color[0], fill_color[1], fill_color[2], fill_color[3]};
    SpriteVertex b = {x + wf, y, 0.0f, tc_x + tc_width, 1.0f - tc_y, fill_color[0], fill_color[1], fill_color[2], fill_color[3]};
    SpriteVertex c = {x + wf, y + hf, 0.0f, tc_x + tc_width, 1.0f - (tc_y - tc_height), fill_color[0], fill_color[1], fill_color[2], fill_color[3]};
    SpriteVertex d = {x, y + hf, 0.0f, tc_x, 1.0f - (tc_y - tc_height), fill_color[0], fill_color[1], fill_color[2], fill_color[3]};

    addQuad(a, b, c, d);
}

void SpriteVertexBuffer::addQuad(SpriteVertex const & a, SpriteVertex const & b, SpriteVertex const & c, SpriteVertex const & d) {
    cpu_buffer.push_back(a);
    cpu_buffer.push_back(b);
    cpu_buffer.push_back(c);

    cpu_buffer.push_back(a);
    cpu_buffer.push_back(c);
    cpu_buffer.push_back(d);
}

void SpriteVertexBuffer::createQuad(float xf, float yf, float wf, float hf, std::array<float, 4> const & color) {
    SpriteVertex a = {xf, yf, 0.0f, 0.0, 0.0, color[0], color[1], color[2], color[3]};
    SpriteVertex b = {xf + wf, yf, 0.0f, 1.0f, 0.0f, color[0], color[1], color[2], color[3]};
    SpriteVertex c = {xf + wf, yf + hf, 0.0f, 1.0f, 1.0f, color[0], color[1], color[2], color[3]};
    SpriteVertex d = {xf, yf + hf, 0.0f, 0.0f, 1.0f, color[0], color[1], color[2], color[3]};

    addQuad(a, b, c, d);
}

void SpriteVertexBuffer::clear() {
    cpu_buffer.clear();
}

}
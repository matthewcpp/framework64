#pragma once

#include "framework64/camera.h"
#include "framework64/desktop/mesh.h"

#include <gl/glew.h>

#include <array>
#include <string>

namespace framework64 {

class MeshRenderer {
public:
    bool init(std::string const & shader_dir);
    void begin(fw64Camera * cam);
    void end();

    void drawStaticMesh(fw64Transform* transform, fw64Mesh* mesh);
    void setupMesh(fw64Mesh* mesh);

private:
    void updateMeshTransformBlock(fw64Transform* transform);
    void setActiveProgram(GLuint program);

private:

    struct LightingData {
        std::array<float, 4> ambient_light_color = {0.1f, 0.1f, 0.1f, 0.1f};
        std::array<float, 4> light_color = {1.0f, 1.0f, 1.0f, 1.0f};
        std::array<float, 3> light_direction = {0.0f, -1.0f, 0.0f};
        float align = 0.0f;
    };

    struct MeshTransformData {
        std::array<float, 16> mvp_matrix;
        std::array<float, 16> normal_matrix;
    };

    template <typename T>
    struct UniformBlock {
        T data;
        GLuint buffer;
        GLuint binding_index = 0;

        void create(GLuint bind_index);
        void update();
    };


    struct GouraudShader {
        GLuint program;
        GLuint lighting_data_uniform_block_index;
        GLuint mesh_transform_uniform_block_index;

        GLint diffuse_color_location;

        bool create(std::string const& shader_dir);
        void setUniforms(fw64Mesh::Material const & material);
    };

private:
    UniformBlock<LightingData> lighting_data_uniform_block;
    UniformBlock<MeshTransformData> mesh_transform_uniform_block;

    std::array<float, 16> view_projection_matrix;

    GouraudShader gouraud_shader;
    fw64Camera* camera = nullptr;
    GLuint active_program = 0;
};

template<typename T>
void MeshRenderer::UniformBlock<T>::create(GLuint bind_index) {
    binding_index = bind_index;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &data, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, binding_index, buffer, 0, sizeof(T));
}

template<typename T>
void MeshRenderer::UniformBlock<T>::update() {
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data);
}

}
#pragma once

#include "framework64/camera.h"
#include "framework64/desktop/mesh.h"
#include "framework64/desktop/shader_cache.h"

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

private:
    void updateMeshTransformBlock(fw64Transform* transform);
    void setActiveShader(ShaderProgram* shader);

private:

    struct LightingData {
        std::array<float, 3> ambient_light_color = {1.0f, 1.0f, 1.0f};
        float ambient_light_intensity = 0.1;
        std::array<float, 3> light_color = {1.0f, 1.0f, 1.0f};
        float align1 = 0.0f;
        std::array<float, 3> light_direction = {0.57735f, 0.57735f, 0.57735f};
        float align2 = 0.0f;
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

private:
    UniformBlock<LightingData> lighting_data_uniform_block;
    UniformBlock<MeshTransformData> mesh_transform_uniform_block;

    std::array<float, 16> view_projection_matrix;

    fw64Camera* camera = nullptr;
    ShaderProgram* active_shader = nullptr;
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
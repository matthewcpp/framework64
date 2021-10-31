#pragma once

#include "framework64/camera.h"
#include "framework64/desktop/mesh.h"
#include "framework64/desktop/shader_cache.h"

#include "framework64/desktop/uniform_block.h"

#include <array>
#include <string>

namespace framework64 {

class MeshRenderer {
public:
    bool init(std::string const & shader_dir);
    void begin(fw64Camera * cam);
    void end();

    void drawStaticMesh(fw64Transform* transform, fw64Mesh* mesh);

    void setDepthTestingEnabled(bool enabled);

public:
    fw64Primitive::Mode render_mode = fw64Primitive::Mode::Unknown;

private:
    void updateMeshTransformBlock(fw64Transform* transform);
    void setActiveShader(ShaderProgram* shader);
    void setGlDepthTestingState();

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

private:
    UniformBlock<LightingData> lighting_data_uniform_block;
    UniformBlock<MeshTransformData> mesh_transform_uniform_block;

    std::array<float, 16> view_projection_matrix;

    fw64Camera* camera = nullptr;
    ShaderProgram* active_shader = nullptr;

    bool depth_testing_enabled = 1;
};


}
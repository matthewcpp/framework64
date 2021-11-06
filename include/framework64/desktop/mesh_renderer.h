#pragma once

#include "framework64/camera.h"
#include "framework64/renderer.h"
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
    inline bool depthTestingEnabled() const { return depth_testing_enabled; }

    void setAmbientLightColor(uint8_t r, uint8_t g, uint8_t b);
    void setLightEnabled(int index, int enabled);
    void setLightDirection(int index, float x, float y, float z);
    void setLightColor(int index, uint8_t r, uint8_t g, uint8_t b);

public:
    fw64Primitive::Mode render_mode = fw64Primitive::Mode::Unknown;

private:
    void updateMeshTransformBlock(fw64Transform* transform);
    void updateLightingBlock();
    void setActiveShader(ShaderProgram* shader);
    void setGlDepthTestingState();

private:

    struct Light {
        std::array<float, 4> light_color;
        std::array<float, 4> light_direction;
    };

    struct LightInfo {
        Light light;
        int active;
    };

    struct LightingData {
        std::array<float, 4> ambient_light_color = {0.1f, 0.1f, 0.1f, 1.0f};
        std::array<Light, FW64_RENDERER_MAX_LIGHT_COUNT> lights;
        int light_count;
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

    bool depth_testing_enabled = true;

    std::array<LightInfo, FW64_RENDERER_MAX_LIGHT_COUNT> lights;
    bool lighting_dirty = false;
};

}
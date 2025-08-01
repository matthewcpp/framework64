#pragma once

#include "framework64/renderer.h"

#include "framework64/desktop/display.hpp"
#include "framework64/desktop/framebuffer.hpp"
#include "framework64/desktop/mesh.hpp"
#include "framework64/desktop/render_pass.hpp"
#include "framework64/desktop/shader_cache.hpp"
#include "framework64/desktop/sprite_batch.hpp"
#include "framework64/desktop/uniform_block.hpp"

#include <SDL2/SDL.h>

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

#include <array>
#include <limits>
#include <string>
#include <vector>

struct fw64Renderer {
public: 
    fw64Renderer(fw64Display& display, framework64::ShaderCache& shader_cache)
        : sprite_material(shader_cache), display(display), shader_cache(shader_cache) {}

public:
    bool init(int width, int height);

    void setCamera(fw64Camera* cam);
    void setViewMatrices(float* projection, float* view);
    void setViewport(fw64Viewport const * viewport);

    void clearViewport(fw64Viewport const & viewport, fw64ClearFlags flags);

    void submitRenderpass(fw64RenderPass* renderpass);

    void beginFrame();
    void endFrame();

private:
    bool initDisplay(int width, int height);
    bool initFramebuffer(int width, int height);

private:
    void drawSpriteBatch(fw64SpriteBatch* spritebatch);
    void drawMeshesFromQueue(fw64RenderPass* renderpass, fw64ShadingMode index);
    void drawRenderPass(fw64RenderPass* renderpass);
    

public:
    void setFogEnabled(bool enabled);
    [[nodiscard]] inline bool fogEnabled() const { return fog_enabled; }
    void setFogPositions(float fog_min, float fog_max);
    void setFogColor(float r, float g, float b);

private:     
    void updateMeshTransformBlock(float* matrix);
    void updateLightingBlock(const LightingInfo& lighting_info);
    void setActiveShader(framework64::ShaderProgram* shader);
    void drawPrimitive(fw64Primitive const & primitive, const fw64Material* material);

    void fogValuesChanged();

private:
    struct MeshTransformData {
        std::array<float, 16> mvp_matrix;
        std::array<float, 16> normal_matrix;
        float camera_near;
        float camera_far;
    };

    struct FogData {
        std::array<float, 4> fog_color = {0.33f, 0.33f, 0.33f, 1.0f};
        float min_distance = std::numeric_limits<float>::max();
        float max_distance = std::numeric_limits<float>::max();
    };

private:
    struct ViewportRect{ GLint x, y; GLsizei width, height; };
    ViewportRect getViewportRect(fw64Viewport const * viewport) const;

private:

    struct LightingData {
        std::array<float, 4> ambient_light_color = {0.1f, 0.1f, 0.1f, 1.0f};
        std::array<Light, FW64_RENDERER_MAX_LIGHT_COUNT> lights;
        int light_count;
    };

    framework64::UniformBlock<LightingData> lighting_data_uniform_block;
    framework64::UniformBlock<MeshTransformData> mesh_transform_uniform_block;
    framework64::UniformBlock<FogData> fog_data_uniform_block;

    std::array<float, 16> view_matrix, projection_matrix, view_projection_matrix;
    framework64::ShaderProgram* active_shader = nullptr;

    float fog_min_distance = 0.4f;
    float fog_max_distance = 0.8f;

    bool fog_enabled = false;

public:
    fw64Material sprite_material;

    fw64Camera* current_camera = nullptr;
private:
    fw64Display& display;
    framework64::ShaderCache& shader_cache;
    framework64::Framebuffer framebuffer;
    std::vector<fw64RenderPass*> renderpasses;

    /** holds the index of the renderpass currently being processed. */
    size_t renderpass_index;
};
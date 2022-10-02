#pragma once

#include "framework64/renderer.h"

#include "framework64/desktop/shader_cache.h"
#include "framework64/desktop/pixel_texture.h"
#include "framework64/desktop/screen_overlay.h"
#include "framework64/desktop/sprite_batch.h"
#include "framework64/desktop/uniform_block.h"

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

struct fw64Framebuffer {
    framework64::PixelTexture texture;
};

struct fw64Renderer {
public:
    bool init(int width, int height, framework64::ShaderCache& shader_cache);

    void setClearColor(float r, float g, float b, float a);
    void begin(fw64RenderMode new_render_mode, fw64RendererFlags flags);
    void setCamera(fw64Camera* cam);
    void end(fw64RendererFlags flags);
    void setScreenSize(int width, int height);

    void clearViewport(fw64Camera* camera, fw64RendererFlags flags);
    IVec2 getViewportSize(fw64Camera* camera);

private:
    bool initDisplay(int width, int height);
    void initLighting();

public:
    void drawStaticMesh(fw64Mesh* mesh, fw64Transform* transform);
    void drawAnimatedMesh(fw64Mesh* mesh, fw64AnimationController* controller, fw64Transform* transform);

public:
    void drawFilledRect(float x, float y, float width, float height);
    void drawSprite(fw64Texture* texture, float x, float y);
    void drawSpriteFrame(fw64Texture* texture, int frame, float x, float y, float scale_x, float scale_y);
    void drawText(fw64Font* font, float x, float y, const char* text, uint32_t count);

public:
    void setDepthTestingEnabled(bool enabled);
    [[nodiscard]] inline bool depthTestingEnabled() const { return depth_testing_enabled; }

public:
    void setFogEnabled(bool enabled);
    [[nodiscard]] inline bool fogEnabled() const { return fog_enabled; }
    void setFogPositions(float fog_min, float fog_max);
    void setFogColor(float r, float g, float b);

public:
    void setAmbientLightColor(uint8_t r, uint8_t g, uint8_t b);
    void setLightEnabled(int index, int enabled);
    void setLightDirection(int index, float x, float y, float z);
    void setLightColor(int index, uint8_t r, uint8_t g, uint8_t b);

    void renderFullscreenOverlay(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

private:     
    void updateMeshTransformBlock(fw64Matrix & matrix);
    void updateLightingBlock();
    void setActiveShader(framework64::ShaderProgram* shader);
    void setGlDepthTestingState();
    void drawPrimitive(fw64Primitive const & primitive);

    void fogValuesChanged();

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
        float camera_near;
        float camera_far;
    };

    struct FogData {
        std::array<float, 4> fog_color = {0.33f, 0.33f, 0.33f, 1.0f};
        float min_distance = std::numeric_limits<float>::max();
        float max_distance = std::numeric_limits<float>::max();
    };

private:
    enum DrawingMode { None, Mesh, Rect };
    void setDrawingMode(DrawingMode mode);

private:
    struct ViewportRect{ GLint x, y; GLsizei width, height; };
    ViewportRect getViewportRect(fw64Camera* camera) const;

private:
    framework64::UniformBlock<LightingData> lighting_data_uniform_block;
    framework64::UniformBlock<MeshTransformData> mesh_transform_uniform_block;
    framework64::UniformBlock<FogData> fog_data_uniform_block;

    std::array<float, 16> view_projection_matrix;
    framework64::ShaderProgram* active_shader = nullptr;
    std::array<LightInfo, FW64_RENDERER_MAX_LIGHT_COUNT> lights;
    framework64::ScreenOverlay screen_overlay;

    float fog_min_distance = 0.4f;
    float fog_max_distance = 0.8f;

    bool depth_testing_enabled = true;
    bool fog_enabled = false;
    bool fog_dirty = true;
    bool lighting_dirty = false;

public:
    framework64::SpriteBatch sprite_batch;
    fw64Primitive::Mode primitive_type;
    DrawingMode drawing_mode = DrawingMode::None;
    int screen_width;
    int screen_height;
    std::array<float, 4> clear_color = {0.0f, 0.0f, 0.0f, 1.0f};

    // Note: This is not currently implemented for desktop rendering
    bool anti_aliasing_enabled = true;

    fw64Framebuffer framebuffer_write_texture;
    fw64RendererPostDrawFunc post_draw_callback = nullptr;
    void* post_draw_callback_arg = nullptr;

    fw64Camera* current_camera = nullptr;

private:
    SDL_Window* window;
    SDL_GLContext gl_context;
};
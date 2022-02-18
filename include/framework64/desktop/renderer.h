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
#include <string>
#include <vector>

struct fw64Framebuffer {
    framework64::PixelTexture texture;
};

struct fw64Renderer {
public:
    bool init(int width, int height, framework64::ShaderCache& shader_cache);

    void setClearColor(float r, float g, float b, float a);
    void begin(fw64Camera* cam, fw64RenderMode new_render_mode, fw64RendererFlags flags);
    void end(fw64RendererFlags flags);
    void setScreenSize(int width, int height);

private:
    bool initDisplay(int width, int height);
    void initLighting();

public:
    void drawStaticMesh(fw64Mesh* mesh, fw64Transform* transform);
    void drawAnimatedMesh(fw64Mesh* mesh, fw64AnimationController* controller, fw64Transform* transform);

public:
    void drawSprite(fw64Texture* texture, float x, float y);
    void drawSpriteFrame(fw64Texture* texture, int frame, float x, float y, float scale_x, float scale_y);
    void drawText(fw64Font* font, float x, float y, const char* text, uint32_t count);

public:
    void setDepthTestingEnabled(bool enabled);
    inline bool depthTestingEnabled() const { return depth_testing_enabled; }

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

    enum DrawingMode {
        None, Mesh, Rect
    };

    void setDrawingMode(DrawingMode mode);

private:
    framework64::UniformBlock<LightingData> lighting_data_uniform_block;
    framework64::UniformBlock<MeshTransformData> mesh_transform_uniform_block;

    std::array<float, 16> view_projection_matrix;

    framework64::ShaderProgram* active_shader = nullptr;

    bool depth_testing_enabled = true;

    std::array<LightInfo, FW64_RENDERER_MAX_LIGHT_COUNT> lights;
    bool lighting_dirty = false;

    framework64::ScreenOverlay screen_overlay;

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

    fw64Camera* camera = nullptr;

private:
    SDL_Window* window;
    SDL_GLContext gl_context;
};
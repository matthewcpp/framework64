#include "framework64/desktop/renderer.hpp"

#include "framework64/matrix.h"

#include <cassert>
#include <iostream>

bool fw64Renderer::init(int width, int height, framework64::ShaderCache& shader_cache) {
    if (!initFramebuffer(width, height))
        return false;

    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    mesh_transform_uniform_block.create();
    fog_data_uniform_block.create();

    initLighting();

    if (!sprite_batch.init(shader_cache)) return false;

    setScreenSize(width, height);

    screen_overlay.init(shader_cache);

    return true;
}

bool fw64Renderer::initFramebuffer(int width, int height) {
    if (!framebuffer.initialize(width, height)) {
        std::cout << "Failed to initialize framebuffer" << std::endl;
        return false;
    }
    return true;
}

void fw64Renderer::initLighting() {
    lights[0].light.light_direction = {0.57735f, 0.57735f, 0.57735f, 1.0f};
    lights[0].light.light_color = {1.0f, 1.0f, 1.0f, 1.0f};
    lights[0].active = 1;

    lights[1].light.light_direction = {-0.57735f, -0.57735f, 0.57735f, 1.0f};
    lights[1].light.light_color = {1.0f, 1.0f, 1.0f, 1.0f};
    lights[1].active = 0;

    lighting_data_uniform_block.create();

    lighting_dirty = true;
}

void fw64Renderer::setClearColor(float r, float g, float b, float a) {
    clear_color = {r, g, b, a};
    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
}

void fw64Renderer::beginFrame() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.framebuffer_handle);
}

void fw64Renderer::endFrame() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.framebuffer_handle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(  0, 0, framebuffer.width, framebuffer.height,
                        0, 0, display.width(), display.height(),
                        GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    display.swap();
}

void fw64Renderer::begin(fw64PrimitiveMode mode, fw64RendererFlags flags) {
    if (lighting_dirty) {
        updateLightingBlock();
        lighting_dirty = false;
    }

    if (fog_dirty) {
        fog_data_uniform_block.update();
        fog_dirty = false;
    }

    if ((flags & FW64_RENDERER_FLAG_CLEAR) == FW64_RENDERER_FLAG_CLEAR) {
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    switch(mode) {
        case FW64_PRIMITIVE_MODE_TRIANGLES:
            primitive_type = fw64Primitive::Mode::Triangles;
            break;
        case FW64_PRIMITIVE_MODE_LINES:
            primitive_type = fw64Primitive::Mode::Lines;
            break;
        default:
            break;
    }

    sprite_batch.newFrame();
}

fw64Renderer::ViewportRect fw64Renderer::getViewportRect(fw64Camera* camera) const {
    float bottom = camera->viewport_pos.y + camera->viewport_size.y;

    return {
        static_cast<GLint>(camera->viewport_pos.x * screen_width),
        static_cast<GLint>((1.0f - bottom) * screen_height),
        static_cast<GLsizei>(camera->viewport_size.x * screen_width),
        static_cast<GLsizei>(camera->viewport_size.y * screen_height)
    };
}

IVec2 fw64Renderer::getViewportSize(fw64Camera* camera) {
    return {
        static_cast<int>(camera->viewport_size.x * screen_width),
        static_cast<int>(camera->viewport_size.y * screen_height)
    };
}

void fw64Renderer::setCamera(fw64Camera* camera) {
    if (camera == current_camera)
        return;

    mesh_transform_uniform_block.data.camera_near = camera->near;
    mesh_transform_uniform_block.data.camera_far = camera->far;

    auto viewport_rect = getViewportRect(camera);

    sprite_batch.flush();
    current_camera = camera;

    matrix_multiply(view_projection_matrix.data(), &current_camera->projection.m[0], &current_camera->view.m[0]);
    sprite_batch.setScreenSize(viewport_rect.width, viewport_rect.height);

    glViewport(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);
}

void fw64Renderer::clearViewport(fw64Camera* camera, fw64RendererFlags flags) {
    glEnable(GL_SCISSOR_TEST);

    auto viewport_rect = getViewportRect(camera);
    glViewport(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);
    glScissor(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);

    GLbitfield clear_flags = 0;
    if (flags & FW64_RENDERER_FLAG_CLEAR_COLOR)
        clear_flags |= GL_COLOR_BUFFER_BIT;

    if (flags & FW64_RENDERER_FLAG_CLEAR_DEPTH)
        clear_flags |= GL_DEPTH_BUFFER_BIT;

    glClear(clear_flags);

    glDisable(GL_SCISSOR_TEST);

    // restore the camera viewport if necessary
    if (camera != current_camera) {
        viewport_rect = getViewportRect(current_camera);
        glViewport(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);
    }
}

void fw64Renderer::setGlDepthTestingState() {
    if (depth_testing_enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void fw64Renderer::end(fw64RendererFlags flags) {
    setDrawingMode(DrawingMode::None);

    current_camera = nullptr;
    active_shader = nullptr;
    primitive_type = primitive_type = fw64Primitive::Mode::Unknown;
}

void fw64Renderer::setScreenSize(int width, int height) {
    screen_width = width;
    screen_height = height;

    sprite_batch.setScreenSize(width, height);
}

void fw64Renderer::updateMeshTransformBlock(fw64Matrix & matrix) {
    // compute the normal matrix: transpose(inverse(modelView))
    matrix_multiply(mesh_transform_uniform_block.data.normal_matrix.data(), &current_camera->view.m[0], &matrix.m[0]);
    matrix_invert(mesh_transform_uniform_block.data.normal_matrix.data(), mesh_transform_uniform_block.data.normal_matrix.data());
    matrix_transpose(mesh_transform_uniform_block.data.normal_matrix.data());

    matrix_multiply(mesh_transform_uniform_block.data.mvp_matrix.data(), view_projection_matrix.data(), &matrix.m[0]);

    mesh_transform_uniform_block.update();
}

void fw64Renderer::drawPrimitive(fw64Primitive const & primitive) {
    setActiveShader(primitive.material->shader);
    active_shader->shader->setUniforms(active_shader, *primitive.material);
    glBindVertexArray(primitive.gl_info.gl_vertex_array_object);

    glDrawElements(primitive.mode, primitive.gl_info.element_count, primitive.gl_info.primitive_mode, 0);
}

void fw64Renderer::drawStaticMesh(fw64Mesh* mesh, fw64Transform* transform) {
    setDrawingMode(DrawingMode::Mesh);
    updateMeshTransformBlock(transform->matrix);

    for (auto const & primitive : mesh->primitives) {
        if (primitive->mode != primitive_type)
            continue;

        drawPrimitive(*primitive);
    }
}

void fw64Renderer::drawAnimatedMesh(fw64Mesh *mesh, fw64AnimationController* controller, fw64Transform *transform) {
    setDrawingMode(DrawingMode::Mesh);

    for (auto const & primitive : mesh->primitives) {
        if (primitive->mode != primitive_type)
            continue;

        // should this be done in the shader?
        fw64Matrix transform_matrix;
        matrix_multiply(&transform_matrix.m[0], &transform->matrix.m[0], &controller->matrices[primitive->joint_index].m[0]);
        updateMeshTransformBlock(transform_matrix);

        drawPrimitive(*primitive);
    }
}

void fw64Renderer::drawSprite(fw64Texture* texture, float x, float y){
    setDrawingMode(DrawingMode::Rect);
    sprite_batch.drawSprite(texture, x, y);
}

void fw64Renderer::drawFilledRect(float x, float y, float width, float height) {
    setDrawingMode(DrawingMode::Rect);
    sprite_batch.drawFilledRect(x, y, width, height);
}

void fw64Renderer::drawSpriteFrame(fw64Texture* texture, int frame, float x, float y, float scale_x, float scale_y) {
    setDrawingMode(DrawingMode::Rect);
    sprite_batch.drawSpriteFrame(texture, frame, x, y, scale_x, scale_y);
}

void fw64Renderer::drawText(fw64Font* font, float x, float y, const char* text, uint32_t count) {
    setDrawingMode(DrawingMode::Rect);
    sprite_batch.drawText(font, x, y, text, count);
}

void fw64Renderer::setActiveShader(framework64::ShaderProgram* shader) {
    if (active_shader != shader) {
        active_shader = shader;

        glUseProgram(active_shader->handle);

        if (active_shader->lighting_data_uniform_block_index != GL_INVALID_INDEX)
            glUniformBlockBinding(active_shader->handle, active_shader->lighting_data_uniform_block_index, lighting_data_uniform_block.binding_index);

        if (active_shader->mesh_transform_uniform_block_index != GL_INVALID_INDEX)
            glUniformBlockBinding(active_shader->handle, active_shader->mesh_transform_uniform_block_index, mesh_transform_uniform_block.binding_index);

        if (active_shader->fog_uniform_block_index != GL_INVALID_INDEX)
            glUniformBlockBinding(active_shader->handle, active_shader->fog_uniform_block_index, fog_data_uniform_block.binding_index);
    }
}

void fw64Renderer::setDepthTestingEnabled(bool enabled) {
    depth_testing_enabled = enabled;

    if (drawing_mode == DrawingMode::Mesh)
        setGlDepthTestingState();
}

void fw64Renderer::updateLightingBlock() {
    int block_index = 0;

    for (auto const & light_info : lights) {
        if (light_info.active) {
            lighting_data_uniform_block.data.lights[block_index++] = light_info.light;
        }
    }

    lighting_data_uniform_block.data.light_count = block_index;
    lighting_data_uniform_block.update();
}

void fw64Renderer::setAmbientLightColor(uint8_t r, uint8_t g, uint8_t b) {
    lighting_data_uniform_block.data.ambient_light_color[0] = static_cast<float>(r) / 255.0f;
    lighting_data_uniform_block.data.ambient_light_color[1] = static_cast<float>(r) / 255.0f;
    lighting_data_uniform_block.data.ambient_light_color[2] = static_cast<float>(r) / 255.0f;

    lighting_dirty = true;
}

void fw64Renderer::setLightEnabled(int index, int enabled) {
    assert(index < FW64_RENDERER_MAX_LIGHT_COUNT);

    lights[index].active = enabled;

    lighting_dirty = true;
}

void fw64Renderer::setLightDirection(int index, float x, float y, float z) {
    assert(index < FW64_RENDERER_MAX_LIGHT_COUNT);

    lights[index].light.light_direction[0] = x;
    lights[index].light.light_direction[1] = y;
    lights[index].light.light_direction[2] = z;

    lighting_dirty = true;
}

void fw64Renderer::setLightColor(int index, uint8_t r, uint8_t g, uint8_t b) {
    assert(index < FW64_RENDERER_MAX_LIGHT_COUNT);

    lights[index].light.light_color[0] = static_cast<float>(r) / 255.0f;
    lights[index].light.light_color[1] = static_cast<float>(g) / 255.0f;
    lights[index].light.light_color[2] = static_cast<float>(b) / 255.0f;

    lighting_dirty = true;
}

void fw64Renderer::renderFullscreenOverlay(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    screen_overlay.primitive.material->color[0] = static_cast<float>(r) / 255.0f;
    screen_overlay.primitive.material->color[1] = static_cast<float>(g) / 255.0f;
    screen_overlay.primitive.material->color[2] = static_cast<float>(b) / 255.0f;
    screen_overlay.primitive.material->color[3] = static_cast<float>(a) / 255.0f;

    matrix_set_identity(mesh_transform_uniform_block.data.normal_matrix.data());
    matrix_set_identity(mesh_transform_uniform_block.data.mvp_matrix.data());
    mesh_transform_uniform_block.update();

    glDisable(GL_DEPTH_TEST);
    drawPrimitive(screen_overlay.primitive);
}

void fw64Renderer::setDrawingMode(DrawingMode mode) {
    if (mode == drawing_mode)
        return;

    // cleanup actions from current drawing mode
    if (drawing_mode == DrawingMode::Rect) {
        sprite_batch.flush();   
    }

    drawing_mode = mode;

    // begin new drawing mode
    switch (drawing_mode) {
        case DrawingMode::Mesh:
            setGlDepthTestingState();
            break;

        case DrawingMode::Rect:
            sprite_batch.start();
            break;

        case DrawingMode::None:
            break;
    };
}

void fw64Renderer::setFogEnabled(bool enabled) {
    fog_enabled = enabled;

    if (enabled)
        setFogPositions(fog_min_distance, fog_max_distance);
    else
        setFogPositions(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
}

void fw64Renderer::setFogPositions(float fog_min, float fog_max) {
    fog_data_uniform_block.data.min_distance = fog_min;
    fog_data_uniform_block.data.max_distance = fog_max;

    fog_dirty = true;
}

void fw64Renderer::setFogColor(float r, float g, float b) {
    fog_data_uniform_block.data.fog_color[0] = r;
    fog_data_uniform_block.data.fog_color[1] = g;
    fog_data_uniform_block.data.fog_color[2] = b;

    fog_dirty = true;
}

// Public C interface

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->setClearColor( r / 255.0f, g / 255.0f, b /255.0f, 1.0f);
}

void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64RendererFlags flags) {
    renderer->begin(primitive_mode, flags);
}

void fw64_renderer_set_camera(fw64Renderer* renderer, fw64Camera* camera) {
    renderer->setCamera(camera);
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags) {
    renderer->end(flags);
}

void fw64_renderer_draw_static_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh) {
    assert(renderer->primitive_type == fw64Primitive::Mode::Triangles || renderer->primitive_type == fw64Primitive::Mode::Lines);
    renderer->drawStaticMesh(mesh, transform);
}

void fw64_renderer_draw_animated_mesh(fw64Renderer* renderer, fw64Mesh* mesh, fw64AnimationController* controller, fw64Transform* transform) {
    assert(renderer->primitive_type == fw64Primitive::Mode::Triangles || renderer->primitive_type == fw64Primitive::Mode::Lines);
    renderer->drawAnimatedMesh(mesh, controller, transform);
}

void fw64_renderer_draw_sprite(fw64Renderer* renderer, fw64Texture* texture, int x, int y) {
    renderer->drawSprite(texture, static_cast<float>(x), static_cast<float>(y));
}

void fw64_renderer_draw_sprite_slice(fw64Renderer* renderer, fw64Texture* texture, int frame, int x, int y) {
    renderer->drawSpriteFrame(texture, frame, static_cast<float>(x), static_cast<float>(y), 1.0f, 1.0f);
}

void fw64_renderer_draw_sprite_slice_transform(fw64Renderer* renderer, fw64Texture* texture, int frame, int x, int y, float scale_x , float scale_y, float rotation) {
    (void)rotation;
    renderer->drawSpriteFrame(texture, frame, static_cast<float>(x), static_cast<float>(y), scale_x, scale_y);
}

void fw64_renderer_set_sprite_scissoring_enabled(fw64Renderer* renderer, int enabled) {
    renderer->sprite_scissor_enabled = static_cast<bool>(enabled);
}

int fw64_renderer_get_sprite_scissoring_enabled(fw64Renderer* renderer) {
    return static_cast<int>(renderer->sprite_scissor_enabled);
}

void fw64_renderer_draw_text(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text) {
    renderer->drawText(font, static_cast<float>(x), static_cast<float>(y), text, std::numeric_limits<uint32_t>::max());
}

void fw64_renderer_draw_text_count(fw64Renderer* renderer, fw64Font* font, int x, int y, const char* text, uint32_t count) {
    renderer->drawText(font, static_cast<float>(x), static_cast<float>(y), text, count);
}

void fw64_renderer_draw_filled_rect(fw64Renderer* renderer, int x, int y, int width, int height) {
    renderer->drawFilledRect(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
}

void fw64_renderer_set_fill_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
    renderer->sprite_batch.setFillColor(r, g, b, a);
}

IVec2 fw64_renderer_get_screen_size(fw64Renderer* renderer) {
    IVec2 screen_size = {renderer->screen_width, renderer->screen_height};

    return screen_size;
}

IVec2 fw64_renderer_get_viewport_size(fw64Renderer* renderer, fw64Camera* camera) {
    return renderer->getViewportSize(camera);
}

fw64Camera* fw64_renderer_get_camera(fw64Renderer* renderer) {
    return renderer->current_camera;
}

void fw64_renderer_set_depth_testing_enabled(fw64Renderer* renderer, int enabled) {
    renderer->setDepthTestingEnabled(enabled);
}

int fw64_renderer_get_depth_testing_enabled(fw64Renderer* renderer) {
    return renderer->depthTestingEnabled();
}

void fw64_renderer_set_ambient_light_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->setAmbientLightColor(r, g, b);
}

void fw64_renderer_set_light_enabled(fw64Renderer* renderer, int index, int enabled) {
    renderer->setLightEnabled(index, enabled);
}

void fw64_renderer_set_light_direction(fw64Renderer* renderer, int index, float x, float y, float z) {
    renderer->setLightDirection(index, x, y, z);
}

void fw64_renderer_set_light_color(fw64Renderer* renderer, int index, uint8_t r, uint8_t g, uint8_t b) {
    renderer->setLightColor(index, r, g, b);
}

void fw64_renderer_set_anti_aliasing_enabled(fw64Renderer* renderer, int enabled) {
    renderer->anti_aliasing_enabled = static_cast<bool>(enabled);
}

int fw64_renderer_get_anti_aliasing_enabled(fw64Renderer* renderer) {
    return static_cast<int>(renderer->anti_aliasing_enabled);
}

void fw64_renderer_set_fog_enabled(fw64Renderer* renderer, int enabled) {
    renderer->setFogEnabled(enabled);
}

int fw64_renderer_get_fog_enabled(fw64Renderer* renderer) {
    return renderer->fogEnabled();
}

void fw64_renderer_set_fog_positions(fw64Renderer* renderer, float fog_min, float fog_max) {
    assert( fog_min >= 0.0f && fog_min <= 1.0f &&
            fog_max >= 0.0f && fog_max <= 1.0f &&
            fog_max >= fog_min);

    renderer->setFogPositions(fog_min, fog_max);
}

void fw64_renderer_set_fog_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->setFogColor(
        static_cast<float>(r) / 255.0f,
        static_cast<float>(g) / 255.0f,
        static_cast<float>(b) / 255.0f
    );
}

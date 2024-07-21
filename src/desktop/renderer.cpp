#include "framework64/desktop/renderer.hpp"

#include "framework64/desktop/render_pass.hpp"

#include "framework64/matrix.h"
#include "framework64/node.h"

#include <cassert>
#include <iostream>

bool fw64Renderer::init(int width, int height) {
    if (!initFramebuffer(width, height)) {
        return false;
    }

    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    sprite_material.texture_frame = FW64_DESKTOP_ENTIRE_TEXTURE_FRAME;
    sprite_material.shader = shader_cache.getShaderProgram(FW64_SHADING_MODE_SPRITE);
    sprite_material.shading_mode = FW64_SHADING_MODE_SPRITE;

    mesh_transform_uniform_block.create();
    fog_data_uniform_block.create();

    initLighting();

    screen_overlay.init();

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
                        0, 0, display.window_width(), display.window_height(),
                        GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    display.swap();
}

void fw64Renderer::begin(fw64PrimitiveMode mode, fw64ClearFlags flags) {
    if (lighting_dirty) {
        updateLightingBlock();
        lighting_dirty = false;
    }

    GLbitfield clear_bits = 0;
    if (flags & FW64_CLEAR_FLAG_COLOR) {
        clear_bits |= GL_COLOR_BUFFER_BIT;
    }

    if (flags & FW64_CLEAR_FLAG_DEPTH) {
        clear_bits |= GL_DEPTH_BUFFER_BIT;
    }

    glClear(clear_bits);

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
}

fw64Renderer::ViewportRect fw64Renderer::getViewportRect(fw64Viewport const * viewport) const {
    float bottom = static_cast<float>(viewport->position.y + viewport->size.y);

    return {
        static_cast<GLint>(viewport->position.x ),
        static_cast<GLint>((framebuffer.height - bottom)),
        static_cast<GLsizei>(viewport->size.x),
        static_cast<GLsizei>(viewport->size.y)
    };
}

void fw64Renderer::setCamera(fw64Camera* camera) {
    if (camera == current_camera)
        return;

    mesh_transform_uniform_block.data.camera_near = camera->near;
    mesh_transform_uniform_block.data.camera_far = camera->far;

    current_camera = camera;

    setViewport(&camera->viewport);
    setViewMatrices(camera->projection.m, camera->view.m);
}

void fw64Renderer::setViewport(fw64Viewport const * viewport) {
    auto viewport_rect = getViewportRect(viewport);
    glViewport(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);
}

void fw64Renderer::setViewMatrices(float* projection, float* view) {
    std::memcpy(projection_matrix.data(), projection, sizeof(float) * 16);
    std::memcpy(view_matrix.data(), view, sizeof(float) * 16);
    matrix_multiply(view_projection_matrix.data(), projection, view);
}

void fw64Renderer::clearViewport(fw64Viewport const & viewport, fw64ClearFlags flags) {
    glEnable(GL_SCISSOR_TEST);

    auto viewport_rect = getViewportRect(&viewport);
    glViewport(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);
    glScissor(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);

    GLbitfield clear_flags = 0;
    if (flags & FW64_CLEAR_FLAG_COLOR)
        clear_flags |= GL_COLOR_BUFFER_BIT;

    if (flags & FW64_CLEAR_FLAG_DEPTH)
        clear_flags |= GL_DEPTH_BUFFER_BIT;

    glClear(clear_flags);

    glDisable(GL_SCISSOR_TEST);

    // restore the camera viewport if necessary
    if (current_camera) {
        viewport_rect = getViewportRect(&current_camera->viewport);
        glViewport(viewport_rect.x, viewport_rect.y, viewport_rect.width, viewport_rect.height);
    }
}

void fw64Renderer::setGlDepthTestingState() {
    if (depth_testing_enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void fw64Renderer::drawRenderPass(fw64RenderPass* renderpass) {
    if (renderpass->clear_flags) {
        glClearColor(renderpass->clear_color[0], renderpass->clear_color[1], renderpass->clear_color[2], 1.0f);
        clearViewport(renderpass->viewport, renderpass->clear_flags);
        glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
    }

    setViewport(&renderpass->viewport);
    setViewMatrices(renderpass->projection_matrix.data(), renderpass->view_matrix.data());
    mesh_transform_uniform_block.data.camera_near = renderpass->camera_near;
    mesh_transform_uniform_block.data.camera_far = renderpass->camera_far;

    if (renderpass->depth_testing_enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    if (renderpass->fog_enabled) {
        setFogEnabled(renderpass->fog_enabled);
        setFogPositions(renderpass->fog_begin, renderpass->fog_end);
        setFogColor(renderpass->fog_color[0], renderpass->fog_color[1], renderpass->fog_color[2]);
        fog_data_uniform_block.update();
    } else if (fog_enabled) {
        setFogEnabled(false);
        fog_data_uniform_block.update();
    }

    for (auto& mesh_instance : renderpass->render_queue.mesh_instances) {
        drawStaticMesh(mesh_instance->mesh, &mesh_instance->node->transform);
    }

    for (auto& skinned_mesh_instance : renderpass->render_queue.skinned_mesh_instances) {
        drawAnimatedMesh(skinned_mesh_instance->skinned_mesh->mesh, &skinned_mesh_instance->controller, &skinned_mesh_instance->mesh_instance.node->transform);
    }

    if (renderpass->render_queue.sprite_batches.size()) {
        setDrawingMode(DrawingMode::Rect);
        for (auto batch : renderpass->render_queue.sprite_batches) {
            drawSpriteBatch(batch);
        }
    }
}

void fw64Renderer::drawSpriteBatch(fw64SpriteBatch* sprite_batch) {
    float matrix[16];
    matrix_set_identity(matrix);

    updateMeshTransformBlock(matrix);
    setActiveShader(sprite_material.shader);

    for (const auto & layer : sprite_batch->layers) {
        for (const auto & batch : layer.batches) {
            sprite_material.texture = batch.first;
            active_shader->shader->setUniforms(active_shader, sprite_material);

            auto& sprite_vertex_buffer = sprite_batch->vertex_buffers[batch.second];
            glBindVertexArray(sprite_vertex_buffer.gl_vertex_array_object);
            glDrawArrays(GL_TRIANGLES, 0, sprite_vertex_buffer.cpu_buffer.size());
            glBindVertexArray(0);
        }
    }
}

void fw64Renderer::end(fw64RendererSwapFlags) {
    setDrawingMode(DrawingMode::None);

    current_camera = nullptr;
    active_shader = nullptr;
    primitive_type = primitive_type = fw64Primitive::Mode::Unknown;
}

void fw64Renderer::updateMeshTransformBlock(float* matrix) {
    // compute the normal matrix: transpose(inverse(modelView))
    matrix_multiply(mesh_transform_uniform_block.data.normal_matrix.data(), view_matrix.data(), matrix);
    matrix_invert(mesh_transform_uniform_block.data.normal_matrix.data(), mesh_transform_uniform_block.data.normal_matrix.data());
    matrix_transpose(mesh_transform_uniform_block.data.normal_matrix.data());

    matrix_multiply(mesh_transform_uniform_block.data.mvp_matrix.data(), view_projection_matrix.data(), matrix);

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
    updateMeshTransformBlock(transform->world_matrix);

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
        float transform_matrix[16];
        matrix_multiply(transform_matrix, transform->world_matrix, &controller->matrices[primitive->joint_index].m[0]);
        updateMeshTransformBlock(transform_matrix);

        drawPrimitive(*primitive);
    }
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
    lighting_data_uniform_block.data.ambient_light_color[1] = static_cast<float>(g) / 255.0f;
    lighting_data_uniform_block.data.ambient_light_color[2] = static_cast<float>(b) / 255.0f;

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

    drawing_mode = mode;

    // begin new drawing mode
    // TODO: can this be removed?
    switch (drawing_mode) {
        case DrawingMode::Mesh:
            setGlDepthTestingState();
            break;

        case DrawingMode::Rect:
            break;

        case DrawingMode::None:
            break;
    };
}

void fw64Renderer::setFogEnabled(bool enabled) {
    fog_enabled = enabled;

    if (enabled) {
        setFogPositions(fog_min_distance, fog_max_distance);
    } else {
        setFogPositions(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    }
}

void fw64Renderer::setFogPositions(float fog_min, float fog_max) {
    fog_min_distance = fog_min;
    fog_max_distance = fog_max;
    fog_data_uniform_block.data.min_distance = fog_min;
    fog_data_uniform_block.data.max_distance = fog_max;
}

void fw64Renderer::setFogColor(float r, float g, float b) {
    fog_data_uniform_block.data.fog_color[0] = r;
    fog_data_uniform_block.data.fog_color[1] = g;
    fog_data_uniform_block.data.fog_color[2] = b;
}

// Public C interface

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    renderer->setClearColor( r / 255.0f, g / 255.0f, b /255.0f, 1.0f);
}

void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64ClearFlags flags) {
    renderer->begin(primitive_mode, flags);
}

void fw64_renderer_set_view_matrices(fw64Renderer* renderer, fw64Matrix* projection, uint16_t*, fw64Matrix* view) {
    renderer->setViewMatrices(projection->m, view->m);
}

void fw64_renderer_set_viewport(fw64Renderer* renderer, fw64Viewport* viewport) {
    renderer->setViewport(viewport);
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererSwapFlags flags) {
    renderer->end(flags);
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

void fw64_renderer_submit_renderpass(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    renderer->drawRenderPass(renderpass);
}
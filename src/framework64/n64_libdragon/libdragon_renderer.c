#include "libdragon_renderer.h"

#include "libdragon_mesh.h"
#include "libdragon_render_pass.h"
#include "libdragon_sprite_batch.h"
#include "libdragon_texture.h"

#include "framework64/vec4.h"

#include <GL/gl_integration.h>

void fw64_libdragon_renderer_init(fw64Renderer* renderer) {
    fw64_renderer_set_clear_color(renderer, 0, 0, 0);
}

void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64ClearFlags clear_flags) {
    // Attach and clear the screen
    renderer->disp = display_get();
    renderer->zbuf = display_get_zbuf();
    rdpq_attach(renderer->disp, renderer->zbuf);

    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1); // colorkey (draw pixel with alpha >= 1)

    gl_context_begin();

    glClearColor(renderer->clear_color[0], renderer->clear_color[1], renderer->clear_color[2], renderer->clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererSwapFlags swap_flags) {
    gl_context_end();
    //Detach the screen
    rdpq_detach_show();
}

static float quad[] = {
    1.0f, 1.0f, 0.0f,      1.0f, 1.0f,
    0.0f, 1.0f, 0.0f,      0.0f, 1.0f,
    0.0f, 0.0f, 0.0f,      0.0f, 0.0f,

    1.0f, 1.0f, 0.0f,      1.0f, 1.0f,
    0.0f, 0.0f, 0.0f,      0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,      1.0f, 0.0f,
};

#define VERTEX_STRIDE sizeof(float) * 5
#define VERTEX_COUNT 6

static void fw64_renderer_draw_sprite_batches(fw64RenderPass* renderpass) {
    static float* vertices = NULL;
    if (!vertices) {
        vertices = fw64_allocator_memalign(fw64_default_allocator(), 16, VERTEX_STRIDE * VERTEX_COUNT);
        memcpy(vertices, &quad[0], VERTEX_STRIDE * VERTEX_COUNT);
    }
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, VERTEX_STRIDE, (void*)(&vertices[0]));
    glTexCoordPointer(2, GL_FLOAT, VERTEX_STRIDE, (void*)(&vertices[3]));

    for (size_t i = 0; i < fw64_dynamic_vector_size(&renderpass->render_queue.sprite_batches); i++){
        fw64SpriteBatch* spritebatch = *((fw64SpriteBatch**)fw64_dynamic_vector_item(&renderpass->render_queue.sprite_batches, i));

        for (size_t j = 0; j < fw64_dynamic_vector_size(&spritebatch->blit_vec); j++){
            SpriteBlitInfo* draw_info = (SpriteBlitInfo*)fw64_dynamic_vector_item(&spritebatch->blit_vec, j);
            glBindTexture(GL_TEXTURE_2D, draw_info->texture->image->gl_handle);
            glPushMatrix();
            glTranslatef(draw_info->pos.x, draw_info->pos.y, 0.0f);
            glScalef(32.0f, 32.0f, 0.0f);
            glDrawArrays(GL_TRIANGLES, 0, VERTEX_COUNT);
            glPopMatrix();
        }
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

static void fw64_renderer_set_client_state(const fw64Primitive* primitive) {
    GLuint offset = 0;
    if (primitive->vertex_attributes & FW64_VERTEX_ATTRIBUTE_POSITION) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, primitive->vertex_stride, (void*)primitive->vertex_data);
        offset += 3;
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (primitive->vertex_attributes & FW64_VERTEX_ATTRIBUTE_TEXCOORD) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, primitive->vertex_stride, (void*)(primitive->vertex_data + offset));
        offset += 2;
    } else {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

static void fw64_renderer_draw_unlit_textured(fw64RenderPass* renderpass) {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    fw64RenderQueueBucket* render_queue_bucket = fw64_render_queue_get_bucket(&renderpass->render_queue, FW64_SHADING_MODE_UNLIT_TEXTURED);
    fw64DynamicVector* queue = &render_queue_bucket->static_;
    for (size_t i = 0; i < fw64_dynamic_vector_size(queue); i++) {
        fw64StaticDrawInfo* draw_info = (fw64StaticDrawInfo*)fw64_dynamic_vector_item(queue, i);
        fw64Material* material = fw64_material_collection_get_material(draw_info->instance->materials, draw_info->index);
        fw64Primitive* primitive = draw_info->instance->mesh->primitives + draw_info->index;

        glPushMatrix();
        glMultMatrixf(draw_info->instance->node->transform.world_matrix);

        fw64_renderer_set_client_state(primitive);
        glBindTexture(GL_TEXTURE_2D, material->texture->image->gl_handle);
        glDrawElements(primitive->mode, primitive->element_count,  GL_UNSIGNED_SHORT, primitive->element_data);
        glPopMatrix();
    }
}

void fw64_renderer_submit_renderpass(fw64Renderer* renderer, fw64RenderPass* renderpass) {
    glViewport(renderpass->viewport.position.x, display_get_height() - (renderpass->viewport.position.y + renderpass->viewport.size.y), renderpass->viewport.size.x, renderpass->viewport.size.y);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(renderpass->projection_matrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(renderpass->view_matrix);

    if (fw64_render_queue_has_items(&renderpass->render_queue, FW64_SHADING_MODE_UNLIT_TEXTURED)) {
        fw64_renderer_draw_unlit_textured(renderpass);
    }

    if (fw64_render_queue_has_sprite_batches(&renderpass->render_queue)) {
        fw64_renderer_draw_sprite_batches(renderpass);
    }
}

void fw64_renderer_set_clear_color(fw64Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    vec4_set((Vec4*)&renderer->clear_color[0], (GLfloat)r / 255.0f, (GLfloat)g / 255.0f, (GLfloat)b / 255.0f, 1.0f);
}
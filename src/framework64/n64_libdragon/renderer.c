#include "framework64/n64_libdragon/renderer.h"
#include "framework64/n64_libdragon/mesh.h"

void fw64_renderer_begin(fw64Renderer* renderer, fw64PrimitiveMode primitive_mode, fw64RendererFlags flags) {

}

void fw64_renderer_end(fw64Renderer* renderer, fw64RendererFlags flags) {

}

void fw64_renderer_set_camera(fw64Renderer* renderer, fw64Camera* camera) {
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(&camera->projection.m[0]);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&camera->view.m[0]);
}

void fw64_renderer_draw_static_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh) {
    fw64_renderer_draw_lit_mesh_textured(renderer, transform, mesh);
}

void fw64_renderer_draw_lit_mesh_textured(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh) {

    glDisable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    

    glPushMatrix();
    glMultMatrixf(&transform->matrix.m[0]);

    fw64MaterialBundle* material_budle = mesh->info.material_bundle;

    for (uint32_t i = 0; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;

        fw64Material* material = material_budle->materials + primitive->info.material_index;
        //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, &material->color[0]);
        //glBindTexture(GL_TEXTURE_2D, material->texture->texture_handle);


        glVertexPointer(3, GL_FLOAT, 0, (void*)primitive->data.positions);
        glNormalPointer(GL_FLOAT, 0, (void*)primitive->data.normals);
        glTexCoordPointer(2, GL_FLOAT, 0, (void*)primitive->data.tex_coords);

        glDrawElements(GL_TRIANGLES, primitive->info.element_count * 3, GL_UNSIGNED_SHORT, primitive->data.elements);
    }

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_LIGHTING);

}

void fw64_renderer_draw_unlit_vertex_color_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh) {
    glDisable(GL_LIGHTING);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    glDisable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glPushMatrix();
    glMultMatrixf(&transform->matrix.m[0]);

    for (uint32_t i = 0; i < mesh->info.primitive_count; i++) {
        fw64Primitive* primitive = mesh->primitives + i;
        glVertexPointer(3, GL_FLOAT, 0, (void*)primitive->data.positions);
        glColorPointer(4, GL_UNSIGNED_BYTE,0, (void*)primitive->data.colors);

        glDrawElements(GL_TRIANGLES, primitive->info.element_count * 3, GL_UNSIGNED_SHORT, primitive->data.elements);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glPopMatrix();

    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);
}
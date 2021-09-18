#include "framework64/desktop/mesh.h"

#include <cassert>

fw64Mesh::fw64Mesh() {
    box_invalidate(&bounding_box);
}

void fw64Mesh::freeGlResources(){
    for (auto const & primitive : primitives) {
        glDeleteVertexArrays(1, &primitive.gl_vertex_array_object);
        glDeleteBuffers(1, &primitive.gl_array_buffer_object);
        glDeleteBuffers(1, &primitive.gl_element_buffer_object);
    }
}

// C interface

void fw64_mesh_get_bounding_box(fw64Mesh* mesh, Box* box) {
    *box = mesh->bounding_box;
}

fw64Primitive* fw64_mesh_get_primitive(fw64Mesh* mesh, int index) {
    assert(index >= 0 && index < mesh->primitives.size());

    return &mesh->primitives[index];
}

fw64Material* fw64_mesh_primitive_get_material(fw64Primitive* primitive) {
    return &primitive->material;
}

void fw64_mesh_delete(fw64Mesh* mesh) {
    mesh->freeGlResources();
    delete mesh;
}

int fw64_mesh_get_texture_count(fw64Mesh* mesh) {
    return static_cast<int>(mesh->textures.size());
}

fw64Texture* fw64_mesh_get_texture(fw64Mesh* mesh, int index) {
    assert(index >= 0 && index < mesh->textures.size());
    return mesh->textures[index].get();
}
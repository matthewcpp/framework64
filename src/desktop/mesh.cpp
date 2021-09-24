#include "framework64/desktop/mesh.h"

#include "framework64/desktop/asset_database.h"
#include "framework64/desktop/glb_parser.h"

#include <cassert>

fw64Mesh::fw64Mesh() {
    box_invalidate(&bounding_box);
}

fw64Mesh* fw64Mesh::loadFromDatabase(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->database.select_mesh_statement);
    sqlite3_bind_int(database->database.select_mesh_statement, 1, index);

    if(sqlite3_step(database->database.select_mesh_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->database.select_mesh_statement, 0));
    const std::string mesh_path = database->asset_dir + asset_path;

    framework64::GlbParser glb(database->shader_cache);
    auto* mesh = glb.parseStaticMesh(mesh_path);

    return mesh;
}

void fw64Mesh::freeGlResources(){
    for (auto const & primitive : primitives) {
        glDeleteVertexArrays(1, &primitive.gl_vertex_array_object);
        glDeleteBuffers(1, &primitive.gl_array_buffer_object);
        glDeleteBuffers(1, &primitive.gl_element_buffer_object);
    }
}

// C interface

fw64Mesh* fw64_mesh_load(fw64AssetDatabase* assets, uint32_t index) {
    return fw64Mesh::loadFromDatabase(assets, index);
}

void fw64_mesh_delete(fw64Mesh* mesh) {
    mesh->freeGlResources();
    delete mesh;
}

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

int fw64_mesh_get_texture_count(fw64Mesh* mesh) {
    return static_cast<int>(mesh->textures.size());
}

fw64Texture* fw64_mesh_get_texture(fw64Mesh* mesh, int index) {
    assert(index >= 0 && index < mesh->textures.size());
    return mesh->textures[index].get();
}
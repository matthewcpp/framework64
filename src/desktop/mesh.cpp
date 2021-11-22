#include "framework64/desktop/mesh.h"

#include "framework64/desktop/asset_database.h"
#include "framework64/desktop/glb_parser.h"
#include "framework64/desktop/json_map.h"
#include "framework64/desktop/texture.h"

#include <cassert>

fw64Mesh::fw64Mesh() {
    box_invalidate(&bounding_box);
}

fw64Mesh* fw64Mesh::loadFromDatabase(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->select_mesh_statement);
    sqlite3_bind_int(database->select_mesh_statement, 1, index);

    if(sqlite3_step(database->select_mesh_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_mesh_statement, 0));
    const std::string mesh_path = database->asset_dir + asset_path;


    auto* joint_map_str = reinterpret_cast<const char *>(sqlite3_column_text(database->select_mesh_statement, 1));

    framework64::JointMap joint_map = [&joint_map_str](){
        if (joint_map_str)
            return framework64::parseJointMap(joint_map_str);
        else
            return framework64::JointMap();
    }();

    framework64::GlbParser glb(database->shader_cache);
    auto* mesh = glb.loadStaticMesh(mesh_path, joint_map);

    return mesh;
}

fw64Mesh::~fw64Mesh(){
    for (auto const & primitive : primitives) {
        glDeleteVertexArrays(1, &primitive.gl_vertex_array_object);
        glDeleteBuffers(1, &primitive.gl_array_buffer_object);
        glDeleteBuffers(1, &primitive.gl_element_buffer_object);
    }
}

// C interface

fw64Mesh* fw64_mesh_load(fw64AssetDatabase* assets, uint32_t index, fw64Allocator* allocator) {
    return fw64Mesh::loadFromDatabase(assets, index);
}

void fw64_mesh_delete(fw64AssetDatabase* assets, fw64Mesh* mesh, fw64Allocator* allocator) {
    delete mesh;
}

void fw64_mesh_get_bounding_box(fw64Mesh* mesh, Box* box) {
    *box = mesh->bounding_box;
}

int fw64_mesh_get_primitive_count(fw64Mesh* mesh) {
    return static_cast<int>(mesh->primitives.size());
}

fw64Material* fw64_mesh_get_material_for_primitive(fw64Mesh* mesh, int index) {
    assert(index >= 0 && index < mesh->primitives.size());
    return &mesh->primitives[index].material;
}

Box fw64_mesh_get_bounding_for_primitive(fw64Mesh* mesh, int index) {
    assert(index >= 0 && index < mesh->primitives.size());
    return mesh->primitives[index].bounding_box;
}
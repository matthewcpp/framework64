#include "framework64/desktop/scene.h"

#include "framework64/desktop/asset_database.h"
#include "framework64/desktop/glb_parser.h"
#include "framework64/desktop/json_map.h"

#include <assert.h>
#include <unordered_map>
#include <string>



fw64Scene* fw64_scene_load(fw64AssetDatabase* database, int index) {
    sqlite3_reset(database->select_scene_statement);
    sqlite3_bind_int(database->select_scene_statement, 1, index);

    if(sqlite3_step(database->select_scene_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_scene_statement, 0));
    const std::string scene_path = database->asset_dir + asset_path;

    int scene_index = sqlite3_column_int(database->select_scene_statement, 1);
    int type_map_index = sqlite3_column_int(database->select_scene_statement, 2);
    int layer_map_index = sqlite3_column_int(database->select_scene_statement, 3);

    auto type_map = framework64::load_type_map(database, type_map_index);
    auto layer_map = framework64::load_layer_map(database, layer_map_index);

    framework64::GlbParser glb(database->shader_cache);
    return glb.parseScene(scene_path, scene_index, type_map, layer_map);
}

template <typename T>
std::unordered_map<std::string, T> parseMapJson(std::string const& json_str) {
    std::unordered_map<std::string, T> typemap;
    nlohmann::json json_doc = nlohmann::json ::parse(json_str);

    for (auto const & item : json_doc.items()) {
        typemap[item.key()] = item.value().get<T>();
    }
}

void fw64_scene_delete(fw64AssetDatabase* assets, fw64Scene* scene) {
    (void)assets;
    delete scene;
}

fw64Mesh* fw64_scene_get_mesh(fw64Scene* scene, uint32_t index) {
    assert(index < scene->meshes.size());
    return scene->meshes[index].get();
}

uint32_t fw64_scene_get_mesh_count(fw64Scene* scene) {
    return static_cast<uint32_t>(scene->meshes.size());
}

fw64Node* fw64_scene_get_node(fw64Scene* scene, uint32_t index) {
    assert(index < scene->nodes.size());
    return scene->nodes[index].get();
}

uint32_t fw64_scene_get_node_count(fw64Scene* scene) {
    return static_cast<uint32_t>(scene->nodes.size());
}
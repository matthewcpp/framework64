#include "framework64/desktop/scene.h"

#include "framework64/desktop/asset_database.h"
#include "framework64/desktop/json_map.h"

#include <algorithm>
#include <assert.h>
#include <unordered_map>
#include <string>


fw64Scene* fw64_scene_load(fw64AssetDatabase* database, int index, fw64Allocator* allocator) {
    sqlite3_reset(database->select_scene_statement);
    sqlite3_bind_int(database->select_scene_statement, 1, index);

    if(sqlite3_step(database->select_scene_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_scene_statement, 0));
    const std::string scene_path = database->asset_dir + asset_path;

    int scene_index = sqlite3_column_int(database->select_scene_statement, 1);
    int layer_map_index = sqlite3_column_int(database->select_scene_statement, 2);

    auto layer_map = framework64::load_layer_map(database, layer_map_index);

    auto* scene = database->glb_parser.loadScene(scene_path, scene_index, layer_map);
    scene->calculateBounding();

    scene->allocator = allocator;
    
    return scene;
}

template <typename T>
std::unordered_map<std::string, T> parseMapJson(std::string const& json_str) {
    std::unordered_map<std::string, T> typemap;
    nlohmann::json json_doc = nlohmann::json ::parse(json_str);

    for (auto const & item : json_doc.items()) {
        typemap[item.key()] = item.value().get<T>();
    }
}

void fw64_scene_delete(fw64AssetDatabase* assets, fw64Scene* scene, fw64Allocator* allocator) {
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

Box* fw64_scene_get_initial_bounds(fw64Scene* scene) {
    return &scene->bounding_box;
}

void fw64Scene::calculateBounding() {
    if (nodes.empty()) {
        vec3_set_all(&bounding_box.min, -1.0f);
        vec3_set_all(&bounding_box.max, 1.0f);
        return;
    }

    box_invalidate(&bounding_box);

    for (auto const & node : nodes){
        if (!node->collider || node->collider->type == FW64_COLLIDER_NONE) {
            continue;
        }

        box_encapsulate_box(&bounding_box, &node->collider->bounding);
    }
}

fw64Allocator* fw64_scene_get_allocator(fw64Scene* scene) {
    return scene->allocator;
}

void fw64_scene_update_bounding(fw64Scene* scene) {
    scene->calculateBounding();
}

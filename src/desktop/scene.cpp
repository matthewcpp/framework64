#include "framework64/desktop/scene.h"

#include "framework64/desktop/asset_database.h"
#include "framework64/desktop/glb_parser.h"

#include <assert.h>
#include <unordered_map>
#include <string>


std::unordered_map<std::string, int> parseTypemap(std::string const & typemapJson);

fw64Scene* fw64_scene_load(fw64AssetDatabase* database, int index) {
    sqlite3_reset(database->select_scene_statement);
    sqlite3_bind_int(database->select_scene_statement, 1, index);

    if(sqlite3_step(database->select_scene_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_scene_statement, 0));
    const std::string scene_path = database->asset_dir + asset_path;

    std::string typemapStr = reinterpret_cast<const char *>(sqlite3_column_text(database->select_scene_statement, 1));
    auto const typemap = parseTypemap(typemapStr);

    framework64::GlbParser glb(database->shader_cache);
    return glb.parseScene(scene_path, typemap);
}

std::unordered_map<std::string, int> parseTypemap(std::string const & typemapJson) {
    std::unordered_map<std::string, int> typemap;
    nlohmann::json json_doc = nlohmann::json ::parse(typemapJson);

    for (auto const & item : json_doc.items()) {
        typemap[item.key()] = item.value().get<int>();
    }

    return typemap;
}

void fw64_scene_delete(fw64Scene* scene) {
    delete scene;
}

fw64Mesh* fw64_scene_get_mesh(fw64Scene* scene, uint32_t index) {
    assert(index < scene->meshes.size());
    return scene->meshes[index];
}

uint32_t fw64_scene_get_mesh_count(fw64Scene* scene) {
    return static_cast<uint32_t>(scene->meshes.size());
}

fw64Transform* fw64_scene_get_transform(fw64Scene* scene) {
    return &scene->transform;
}

uint32_t fw64_scene_get_extra_count(fw64Scene* scene){
    return static_cast<uint32_t>(scene->extras.size());
}

fw64SceneExtra* fw64_scene_get_extras(fw64Scene* scene) {
    return scene->extras.data();
}
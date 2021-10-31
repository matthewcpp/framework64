#include "framework64/util/terrain.h"

#include "framework64/desktop/asset_database.h"
#include "framework64/desktop/glb_parser.h"

#include <algorithm>
#include <cassert>

struct fw64Terrain {
    fw64Transform transform;
    std::vector<fw64Mesh*> meshes;
};

fw64Terrain* fw64_terrain_load(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->select_terrain_statement);
    sqlite3_bind_int(database->select_terrain_statement, 1, index);

    if(sqlite3_step(database->select_terrain_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_terrain_statement, 0));
    const std::string terrain_path = database->asset_dir + asset_path;

    framework64::GlbParser glb(database->shader_cache);
    auto const meshes = glb.parseStaticMeshes(terrain_path);

    auto* terrain = new fw64Terrain();
    fw64_transform_init(&terrain->transform);

    std::copy(meshes.begin(), meshes.end(), std::back_inserter(terrain->meshes));


    return terrain;
}

void fw64_terrain_delete(fw64AssetDatabase* assets, fw64Terrain* terrain) {
    for (size_t i = 0; i < terrain->meshes.size(); i++)
        fw64_mesh_delete(assets, terrain->meshes[i]);

    delete terrain;
}

uint32_t fw64_terrain_get_mesh_count(fw64Terrain* terrain) {
    return static_cast<uint32_t>(terrain->meshes.size());
}

fw64Mesh* fw64_terrain_get_mesh(fw64Terrain* terrain, uint32_t index) {
    assert(index >= 0 && index < terrain->meshes.size());
    return terrain->meshes[index];
}

fw64Transform* fw64_terrain_get_transform(fw64Terrain* terrain) {
    return &terrain->transform;
}

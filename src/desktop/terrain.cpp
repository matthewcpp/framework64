#include "framework64/util/terrain.h"

#include "framework64/desktop/asset_database.h"
#include "framework64/desktop/glb_parser.h"

#include <algorithm>

fw64Terrain* fw64_terrain_load(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->select_terrain_statement);
    sqlite3_bind_int(database->select_terrain_statement, 1, index);

    if(sqlite3_step(database->select_terrain_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_terrain_statement, 0));
    const std::string terrain_path = database->asset_dir + asset_path;

    framework64::GlbParser glb(database->shader_cache);
    auto const meshes = glb.parseStaticMeshes(terrain_path);

    fw64Terrain* terrain = new fw64Terrain();
    fw64_transform_init(&terrain->transform);

    terrain->meshes = new fw64Mesh*[meshes.size()];
    std::copy(meshes.begin(), meshes.end(), terrain->meshes);
    terrain->mesh_count = static_cast<uint32_t>(meshes.size());


    return terrain;
}

void fw64_terrain_delete(fw64Terrain* terrain) {
    for (size_t i = 0; i < terrain->mesh_count; i++)
        fw64_mesh_delete(terrain->meshes[i]);

    delete[] terrain->meshes;
    delete terrain;
}

#include "framework64/n64_libdragon/asset_database.h"

#include "framework64/n64_libdragon/asset_bundle_reader.h"
#include "framework64/mesh.h"

int fw64_n64_libdragon_asset_database_init(fw64AssetDatabase* asset_database) {
    fw64_asset_bundle_reader_init(&asset_database->reader);
    return fw64_asset_bundle_reader_open(&asset_database->reader, "rom://assets.data");
}

fw64Mesh* fw64_assets_load_mesh(fw64AssetDatabase* asset_database, fw64AssetId asset_index, fw64Allocator* allocator) {
    if (!fw64_asset_bundle_reader_reset(&asset_database->reader, asset_index)) {
        return NULL;
    }

    return fw64_mesh_load_from_datasource(asset_database, &asset_database->reader.datasource, allocator);
}
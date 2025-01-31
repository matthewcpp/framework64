#pragma once

#include "framework64/asset_database.h"

#include "libdragon_dfs_datasource.h"

#include <libdragon.h>

#define DATASOURCE_COUNT 2

struct fw64AssetDatabase {
    fw64DfsDataSource datasources[DATASOURCE_COUNT];
};

void fw64_libdragon_asset_database_init(fw64AssetDatabase* asset_database);
fw64DfsDataSource* fw64_libdragon_asset_database_open_asset(fw64AssetDatabase* asset_database, fw64AssetId asset_id);

sprite_t* fw64_assets_load_sprite(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator);

void fw64_libdragon_asset_database_filepath(fw64AssetId asset_id, char* buffer);

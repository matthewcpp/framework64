#pragma once

#include "framework64/asset_database.h"

#include "libdragon_dfs_datasource.h"

struct fw64AssetDatabase {
    fw64DfsDataSource datasource;
};

void fw64_libdragon_asset_database_init(fw64AssetDatabase* asset_database);
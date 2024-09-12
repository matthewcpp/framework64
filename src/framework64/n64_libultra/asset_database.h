#pragma once

#include "framework64/asset_database.h"

#include "framework64/n64_libultra/filesystem.h"

struct fw64AssetDatabase{
    fw64N64FilesystemDataSource data_sources[FW64_ASSETS_MAX_OPEN_DATASOURCES];
};

void fw64_n64_asset_database_init(fw64AssetDatabase* assets);


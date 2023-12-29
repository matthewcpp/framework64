#pragma once

#include "framework64/asset_database.h"
#include "framework64/n64_libdragon/asset_bundle_reader.h"

#include <stdint.h>
#include <stdio.h>

typedef struct fw64AssetDatabase fw64AssetDatabase;

struct fw64AssetDatabase {
    fw64AssetBundleReader reader;
};

int fw64_n64_libdragon_asset_database_init(fw64AssetDatabase* asset_database);
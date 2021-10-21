#pragma once

#include "framework64/desktop/asset_database.h"

#include <unordered_map>
#include <string>

namespace framework64 {
typedef std::unordered_map<std::string, int> TypeMap;
typedef std::unordered_map<std::string, uint32_t> LayerMap;

TypeMap load_type_map(fw64AssetDatabase* assets, int index);
LayerMap load_layer_map(fw64AssetDatabase* assets, int index);

}
#pragma once

#include "framework64/desktop/asset_database.h"

#include <unordered_map>
#include <string>

namespace framework64 {
typedef std::unordered_map<std::string, uint32_t> TypeMap;
typedef std::unordered_map<std::string, uint32_t> LayerMap;
typedef std::unordered_map<uint32_t, uint32_t> JointMap;

TypeMap load_type_map(fw64AssetDatabase* assets, int index);
LayerMap load_layer_map(fw64AssetDatabase* assets, int index);
JointMap parseJointMap(std::string const & json_str);
}
#pragma once


#include <unordered_map>
#include <string>

struct fw64AssetDatabase;

namespace framework64 {
typedef std::unordered_map<std::string, uint32_t> LayerMap;
typedef std::unordered_map<uint32_t, uint32_t> JointMap;

LayerMap load_layer_map(fw64AssetDatabase* assets, int index);
JointMap parseJointMap(std::string const & json_str);
}

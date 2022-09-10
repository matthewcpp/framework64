#include "framework64/desktop/json_map.h"

#include <nlohmann/json.hpp>

namespace framework64{

template <typename T>
std::unordered_map<std::string, T> parseMapJson(std::string const& json_str);

LayerMap load_layer_map(fw64AssetDatabase* database, int index) {
    sqlite3_reset(database->select_layermap_statement);
    sqlite3_bind_int(database->select_layermap_statement, 1, index);

    if(sqlite3_step(database->select_layermap_statement) != SQLITE_ROW)
        return {};

    std::string layermapStr = reinterpret_cast<const char *>(sqlite3_column_text(database->select_layermap_statement, 0));
    return parseMapJson<uint32_t>(layermapStr);
}

template <typename T>
std::unordered_map<std::string, T> parseMapJson(std::string const& json_str) {
    std::unordered_map<std::string, T> typemap;
    nlohmann::json json_doc = nlohmann::json ::parse(json_str);

    for (auto const & item : json_doc.items()) {
        typemap[item.key()] = item.value().get<T>();
    }

    return typemap;
}

JointMap parseJointMap(std::string const & json_str) {
    JointMap jointMap;

    if (!json_str.empty()) {

    }
    nlohmann::json json_doc = nlohmann::json ::parse(json_str);

    for (auto const & item : json_doc.items()) {
        uint32_t original_index = static_cast<uint32_t>(std::stoi(item.key()));
        uint32_t mapped_index = item.value().get<uint32_t>();

        jointMap[original_index] = mapped_index;
    }

    return jointMap;
}

}
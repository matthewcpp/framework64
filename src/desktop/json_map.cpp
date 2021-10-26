#include "framework64/desktop/json_map.h"

#include <nlohmann/json.hpp>

namespace framework64{

template <typename T>
std::unordered_map<std::string, T> parseMapJson(std::string const& json_str);

TypeMap load_type_map(fw64AssetDatabase* database, int index) {
    sqlite3_reset(database->select_typemap_statement);
    sqlite3_bind_int(database->select_typemap_statement, 1, index);

    if(sqlite3_step(database->select_typemap_statement) != SQLITE_ROW)
        return {};

    std::string typemapStr = reinterpret_cast<const char *>(sqlite3_column_text(database->select_typemap_statement, 0));
    return parseMapJson<uint32_t>(typemapStr);
}

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

}
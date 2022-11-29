#include "framework64/desktop/asset_database.h"

bool fw64AssetDatabase::init(std::string const & database_path) {
    int result = sqlite3_open_v2(database_path.c_str(), &database, SQLITE_OPEN_READONLY, nullptr);

    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path, hslices, vslices, indexMode FROM images WHERE assetId = ?;", -1, &select_image_statement, nullptr);
    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path from palettes WHERE imageId = ?;", -1, &select_palettes_statement, nullptr);
    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path, size, tileWidth, tileHeight, glyphCount, glyphData FROM fonts WHERE assetId = ?;", -1, &select_font_statement, nullptr);
    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path, jointMap FROM meshes WHERE assetId = ?;", -1, &select_mesh_statement, nullptr);
    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path, count FROM soundBanks WHERE assetId = ?;", -1, &select_sound_bank_statement, nullptr);
    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path, count FROM musicBanks WHERE assetId = ?;", -1, &select_music_bank_statement, nullptr);
    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path, size FROM rawFiles WHERE assetId = ?;", -1, &select_raw_file_statement, nullptr);
    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT jsonStr FROM layerMaps WHERE jsonIndex = ?;", -1, &select_layermap_statement, nullptr);
    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path, sceneIndex, layerMap FROM scenes WHERE assetId = ?;", -1, &select_scene_statement, nullptr);
    if (result) {
        return false;
    }

    return true;
}
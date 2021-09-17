#include "framework64/desktop/database.h"

namespace framework64 {

bool Database::init(std::string const & database_path) {
    int result = sqlite3_open_v2(database_path.c_str(), &database, SQLITE_OPEN_READONLY, nullptr);

    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path, hslices, vslices FROM sprites WHERE assetId = ?;", -1, &select_texture_statement, nullptr);
    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path, size, tileWidth, tileHeight, glyphCount, glyphData FROM fonts WHERE assetId = ?;", -1, &select_font_statement, nullptr);
    if (result) {
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path FROM meshes WHERE assetId = ?;", -1, &select_mesh_statement, nullptr);
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

    return true;
}

}
#pragma once

#include <sqlite3.h>

#include <string>

namespace framework64 {

struct Database {
    bool init(std::string const & database_path);

    sqlite3* database = nullptr;
    sqlite3_stmt* select_texture_statement = nullptr;
    sqlite3_stmt* select_font_statement = nullptr;
    sqlite3_stmt* select_mesh_statement = nullptr;
    sqlite3_stmt* select_sound_bank_statement = nullptr;
    sqlite3_stmt* select_music_bank_statement = nullptr;
    sqlite3_stmt* select_raw_file_statement = nullptr;
};

}

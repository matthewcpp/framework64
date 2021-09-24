#include "framework64/desktop/audio_bank.h"

#include "framework64/desktop/asset_database.h"

fw64SoundBank* fw64SoundBank::loadFromDatabase(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->select_sound_bank_statement);
    sqlite3_bind_int(database->select_sound_bank_statement, 1, index);

    if(sqlite3_step(database->select_sound_bank_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_sound_bank_statement, 0));
    const std::string sound_bank_path = database->asset_dir + asset_path + "/";
    auto sound_bank_count = static_cast<uint32_t>(sqlite3_column_int(database->select_sound_bank_statement, 1));

    return new fw64SoundBank(sound_bank_path, sound_bank_count);

}

fw64MusicBank* fw64MusicBank::loadFromDatabase(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->select_music_bank_statement);
    sqlite3_bind_int(database->select_music_bank_statement, 1, index);

    if(sqlite3_step(database->select_music_bank_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_music_bank_statement, 0));
    const std::string music_bank_path = database->asset_dir + asset_path + "/";
    auto music_bank_count = static_cast<uint32_t>(sqlite3_column_int(database->select_music_bank_statement, 1));

    return new fw64MusicBank(music_bank_path, music_bank_count);
}


// C Interface
fw64SoundBank* fw64_sound_bank_load(fw64AssetDatabase* assets, uint32_t handle) {
    return fw64SoundBank::loadFromDatabase(assets, handle);
}

fw64MusicBank* fw64_music_bank_load(fw64AssetDatabase* assets, uint32_t handle) {
    return fw64MusicBank::loadFromDatabase(assets, handle);
}
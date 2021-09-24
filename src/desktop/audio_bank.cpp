#include "framework64/desktop/audio_bank.h"

#include "framework64/desktop/asset_database.h"

fw64SoundBank* fw64SoundBank::loadFromDatabase(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->database.select_sound_bank_statement);
    sqlite3_bind_int(database->database.select_sound_bank_statement, 1, index);

    if(sqlite3_step(database->database.select_sound_bank_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->database.select_sound_bank_statement, 0));
    const std::string sound_bank_path = database->asset_dir + asset_path + "/";
    auto sound_bank_count = static_cast<uint32_t>(sqlite3_column_int(database->database.select_sound_bank_statement, 1));

    auto sound_bank = new fw64SoundBank();
    sound_bank->load(sound_bank_path, sound_bank_count);

    return sound_bank;
}

bool fw64SoundBank::load(std::string path, uint32_t count) {
    base_path = std::move(path);
    sound_count = count;

    return true;
}

fw64MusicBank* fw64MusicBank::loadFromDatabase(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->database.select_music_bank_statement);
    sqlite3_bind_int(database->database.select_music_bank_statement, 1, index);

    if(sqlite3_step(database->database.select_music_bank_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database->database.select_music_bank_statement, 0));
    const std::string music_bank_path = database->asset_dir + asset_path + "/";
    auto music_bank_count = static_cast<uint32_t>(sqlite3_column_int(database->database.select_music_bank_statement, 1));

    auto music_bank = new fw64MusicBank();
    music_bank->load(music_bank_path, music_bank_count);
    return music_bank;
}

bool fw64MusicBank::load(std::string path, uint32_t count){
    track_count = count;
    base_path = path;
    return true;
}


// C Interface
fw64SoundBank* fw64_sound_bank_load(fw64AssetDatabase* assets, uint32_t handle) {
    return fw64SoundBank::loadFromDatabase(assets, handle);
}

fw64MusicBank* fw64_music_bank_load(fw64AssetDatabase* assets, uint32_t handle) {
    return fw64MusicBank::loadFromDatabase(assets, handle);
}
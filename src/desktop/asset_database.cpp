#include "framework64/asset_database.h"

#include "framework64/desktop/asset_database.h"
#include "framework64/desktop/audio_bank.h"

fw64SoundBank* fw64AssetDatabase::getSoundBank(int handle) {
    auto result = sound_banks.find(handle);

    if (result != sound_banks.end())
        return result->second.get();

    sqlite3_reset(database.select_sound_bank_statement);
    sqlite3_bind_int(database.select_sound_bank_statement, 1, handle);

    if(sqlite3_step(database.select_sound_bank_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database.select_sound_bank_statement, 0));
    const std::string sound_bank_path = asset_dir + asset_path + "/";
    auto sound_bank_count = static_cast<uint32_t>(sqlite3_column_int(database.select_sound_bank_statement, 1));

    auto sound_bank = std::make_unique<fw64SoundBank>();
    if (!sound_bank->load(sound_bank_path, sound_bank_count))
        return nullptr;

    sound_banks[handle] = std::move(sound_bank);

    return sound_banks[handle].get();
}

fw64MusicBank* fw64AssetDatabase::getMusicBank(int handle) {
    auto result = music_banks.find(handle);

    if (result != music_banks.end())
        return result->second.get();

    sqlite3_reset(database.select_music_bank_statement);
    sqlite3_bind_int(database.select_music_bank_statement, 1, handle);

    if(sqlite3_step(database.select_music_bank_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database.select_music_bank_statement, 0));
    const std::string music_bank_path = asset_dir + asset_path + "/";
    auto music_bank_count = static_cast<uint32_t>(sqlite3_column_int(database.select_music_bank_statement, 1));

    auto music_bank = std::make_unique<fw64MusicBank>();
    if (!music_bank->load(music_bank_path, music_bank_count))
        return nullptr;

    music_banks[handle] = std::move(music_bank);

    return music_banks[handle].get();
}

// C Interface

fw64SoundBank* fw64_assets_get_sound_bank(fw64AssetDatabase* assets, uint32_t index) {
    return assets->getSoundBank(index);
}

fw64MusicBank* fw64_assets_get_music_bank(fw64AssetDatabase* assets, uint32_t index) {
    return assets->getMusicBank(index);
}

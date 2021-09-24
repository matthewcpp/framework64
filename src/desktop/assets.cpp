#include "framework64/asset_database.h"

#include "framework64/desktop/asset_database.h"
#include "framework64/desktop/audio_bank.h"
#include "framework64/desktop/font.h"
#include "framework64/desktop/image.h"
#include "framework64/desktop/glb_parser.h"

fw64Image* fw64AssetDatabase::getImage(int handle) {
    auto existing_texture = images.find(handle);

    if (existing_texture != images.end())
        return existing_texture->second.get();

    sqlite3_reset(database.select_image_statement);
    sqlite3_bind_int(database.select_image_statement, 1, handle);

    if(sqlite3_step(database.select_image_statement) != SQLITE_ROW)
        return nullptr;

    std::string image_path = reinterpret_cast<const char *>(sqlite3_column_text(database.select_image_statement, 0));
    const std::string asset_path = asset_dir + image_path;
    auto texture =  fw64Image::loadImageFile(asset_path);

    texture->hslices = sqlite3_column_int(database.select_image_statement, 1);
    texture->vslices = sqlite3_column_int(database.select_image_statement, 2);

    images[handle] = std::unique_ptr<fw64Image>(texture);

    return texture;
}

fw64Font* fw64AssetDatabase::getFont(int handle) {
    auto existing_font = fonts.find(handle);

    if (existing_font != fonts.end())
        return existing_font->second.get();

    sqlite3_reset(database.select_font_statement);
    sqlite3_bind_int(database.select_font_statement, 1, handle);

    if(sqlite3_step(database.select_font_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database.select_font_statement, 0));
    const std::string texture_path = asset_dir + asset_path;
    auto image =  fw64Image::loadImageFile(texture_path);

    if (!image)
        return nullptr;

    auto font = new fw64Font();
    font->texture = std::make_unique<fw64Texture>(image);
    font->size = sqlite3_column_int(database.select_font_statement, 1);

    int tile_width = sqlite3_column_int(database.select_font_statement, 2);
    int tile_height = sqlite3_column_int(database.select_font_statement, 3);

    font->texture->image->hslices = font->texture->image->width / tile_width;
    font->texture->image->vslices = font->texture->image->height / tile_height;

    int glyphCount = sqlite3_column_int(database.select_font_statement, 4);
    font->glyphs.resize(glyphCount);
    memcpy(font->glyphs.data(), sqlite3_column_blob(database.select_font_statement, 5), glyphCount * sizeof(fw64FontGlyph));

    fonts[handle] = std::unique_ptr<fw64Font>(font);

    return font;
}

fw64Mesh* fw64AssetDatabase::getMesh(int handle) {
    auto const result = meshes.find(handle);

    if (result != meshes.end())
        return result->second.get();

    sqlite3_reset(database.select_mesh_statement);
    sqlite3_bind_int(database.select_mesh_statement, 1, handle);

    if(sqlite3_step(database.select_mesh_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(database.select_mesh_statement, 0));
    const std::string mesh_path = asset_dir + asset_path;

    framework64::GlbParser glb(shader_cache);
    auto* mesh = glb.parseStaticMesh(mesh_path);

    if (!mesh)
        return nullptr;

    meshes[handle] = std::unique_ptr<fw64Mesh>(mesh);

    return mesh;
}

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

fw64Mesh* fw64_assets_get_mesh(fw64AssetDatabase* assets, uint32_t index) {
    return assets->getMesh(index);
}

fw64Font* fw64_assets_get_font(fw64AssetDatabase* assets, uint32_t index) {
    return assets->getFont(index);
}

fw64Image* fw64_assets_get_image(fw64AssetDatabase* assets, uint32_t index) {
    return assets->getImage(index);
}

fw64SoundBank* fw64_assets_get_sound_bank(fw64AssetDatabase* assets, uint32_t index) {
    return assets->getSoundBank(index);
}

fw64MusicBank* fw64_assets_get_music_bank(fw64AssetDatabase* assets, uint32_t index) {
    return assets->getMusicBank(index);
}

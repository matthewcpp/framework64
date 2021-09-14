#include "framework64/assets.h"

#include "framework64/desktop/assets.h"
#include "framework64/desktop/audio_bank.h"
#include "framework64/desktop/font.h"
#include "framework64/desktop/glb_parser.h"

bool fw64Assets::init() {
    std::string database_path = asset_dir + "assets.db";
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

fw64Texture* fw64Assets::getTexture(int handle) {
    auto existing_texture = textures.find(handle);

    if (existing_texture != textures.end())
        return existing_texture->second.get();

    sqlite3_reset(select_texture_statement);
    sqlite3_bind_int(select_texture_statement, 1, handle);

    if(sqlite3_step(select_texture_statement) != SQLITE_ROW)
        return nullptr;

    std::string sprite_path = reinterpret_cast<const char *>(sqlite3_column_text(select_texture_statement, 0));
    const std::string asset_path = asset_dir + sprite_path;
    auto texture =  fw64Texture::loadImageFile(asset_path);

    texture->hslices = sqlite3_column_int(select_texture_statement, 1);
    texture->vslices = sqlite3_column_int(select_texture_statement, 2);

    textures[handle] = std::unique_ptr<fw64Texture>(texture);

    return texture;
}

fw64Font* fw64Assets::getFont(int handle) {
    auto existing_font = fonts.find(handle);

    if (existing_font != fonts.end())
        return existing_font->second.get();

    sqlite3_reset(select_font_statement);
    sqlite3_bind_int(select_font_statement, 1, handle);

    if(sqlite3_step(select_font_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(select_font_statement, 0));
    const std::string texture_path = asset_dir + asset_path;
    auto texture =  fw64Texture::loadImageFile(texture_path);

    if (!texture)
        return nullptr;

    auto font = new fw64Font();
    font->texture = texture;
    font->size = sqlite3_column_int(select_font_statement, 1);

    int tile_width = sqlite3_column_int(select_font_statement, 2);
    int tile_height = sqlite3_column_int(select_font_statement, 3);

    font->texture->hslices = font->texture->width / tile_width;
    font->texture->vslices = font->texture->height / tile_height;

    int glyphCount = sqlite3_column_int(select_font_statement, 4);
    font->glyphs.resize(glyphCount);
    memcpy(font->glyphs.data(), sqlite3_column_blob(select_font_statement, 5), glyphCount * sizeof(fw64FontGlyph));

    fonts[handle] = std::unique_ptr<fw64Font>(font);

    return font;
}

fw64Mesh* fw64Assets::getMesh(int handle) {
    auto const result = meshes.find(handle);

    if (result != meshes.end())
        return result->second.get();

    sqlite3_reset(select_mesh_statement);
    sqlite3_bind_int(select_mesh_statement, 1, handle);

    if(sqlite3_step(select_mesh_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(select_mesh_statement, 0));
    const std::string mesh_path = asset_dir + asset_path;

    framework64::GlbParser glb(shader_cache);
    auto* mesh = glb.parseStaticMesh(mesh_path);

    if (!mesh)
        return nullptr;

    meshes[handle] = std::unique_ptr<fw64Mesh>(mesh);

    return mesh;
}

fw64SoundBank* fw64Assets::getSoundBank(int handle) {
    auto result = sound_banks.find(handle);

    if (result != sound_banks.end())
        return result->second.get();

    sqlite3_reset(select_sound_bank_statement);
    sqlite3_bind_int(select_sound_bank_statement, 1, handle);

    if(sqlite3_step(select_sound_bank_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(select_sound_bank_statement, 0));
    const std::string sound_bank_path = asset_dir + asset_path + "/";
    auto sound_bank_count = static_cast<uint32_t>(sqlite3_column_int(select_sound_bank_statement, 1));

    auto sound_bank = std::make_unique<fw64SoundBank>();
    if (!sound_bank->load(sound_bank_path, sound_bank_count))
        return nullptr;

    sound_banks[handle] = std::move(sound_bank);

    return sound_banks[handle].get();
}

fw64MusicBank* fw64Assets::getMusicBank(int handle) {
    auto result = music_banks.find(handle);

    if (result != music_banks.end())
        return result->second.get();

    sqlite3_reset(select_music_bank_statement);
    sqlite3_bind_int(select_music_bank_statement, 1, handle);

    if(sqlite3_step(select_music_bank_statement) != SQLITE_ROW)
        return nullptr;

    std::string asset_path = reinterpret_cast<const char *>(sqlite3_column_text(select_music_bank_statement, 0));
    const std::string music_bank_path = asset_dir + asset_path + "/";
    auto music_bank_count = static_cast<uint32_t>(sqlite3_column_int(select_music_bank_statement, 1));

    auto music_bank = std::make_unique<fw64MusicBank>();
    if (!music_bank->load(music_bank_path, music_bank_count))
        return nullptr;

    music_banks[handle] = std::move(music_bank);

    return music_banks[handle].get();
}

// C Interface

fw64Mesh* fw64_assets_get_mesh(fw64Assets* assets, uint32_t index) {
    return assets->getMesh(index);
}

fw64Font* fw64_assets_get_font(fw64Assets* assets, uint32_t index) {
    return assets->getFont(index);
}

fw64Texture* fw64_assets_get_image(fw64Assets* assets, uint32_t index) {
    return assets->getTexture(index);
}

fw64SoundBank* fw64_assets_get_sound_bank(fw64Assets* assets, uint32_t index) {
    return assets->getSoundBank(index);
}

fw64MusicBank* fw64_assets_get_music_bank(fw64Assets* assets, uint32_t index) {
    return assets->getMusicBank(index);
}

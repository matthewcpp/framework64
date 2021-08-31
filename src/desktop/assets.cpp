#include "framework64/assets.h"


#include "framework64/desktop/assets.h"
#include "framework64/desktop/font.h"
#include "framework64/desktop/texture.h"

#include <SDL_image.h>


fw64Assets::fw64Assets(std::string asset_dir_path) {
    asset_dir = std::move(asset_dir_path);
    database = nullptr;
    select_texture_statement = nullptr;
}

bool fw64Assets::init() {
    int result = IMG_Init(IMG_INIT_PNG);

    if ((result & IMG_INIT_PNG) != IMG_INIT_PNG) {
        return false;
    }

    std::string database_path = asset_dir + "assets.db";
    result = sqlite3_open_v2(database_path.c_str(), &database, SQLITE_OPEN_READONLY, nullptr);

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

bool fw64Assets::isLoaded(int handle) {
    return textures.count(handle) || fonts.count(handle);
}

int fw64_assets_is_loaded(fw64Assets* assets, uint32_t index) {
    return assets->isLoaded(index);
}

fw64Mesh* fw64_assets_get_mesh(fw64Assets* assets, uint32_t index) {
    return nullptr;
}

fw64Font* fw64_assets_get_font(fw64Assets* assets, uint32_t index) {
    return assets->getFont(index);
}

fw64Texture* fw64_assets_get_image(fw64Assets* assets, uint32_t index) {
    return assets->getTexture(index);
}
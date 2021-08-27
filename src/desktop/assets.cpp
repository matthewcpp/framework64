#include "framework64/assets.h"

#include "framework64/desktop/assets.h"
#include "framework64/desktop/texture.h"

#include <SDL_image.h>


fw64Assets::fw64Assets(std::string asset_dir_path) {
    asset_dir = std::move(asset_dir_path);
    database = nullptr;
    select_asset_statement = nullptr;
}

bool fw64Assets::init() {
    int result = IMG_Init(IMG_INIT_PNG);

    if ((result & IMG_INIT_PNG) != IMG_INIT_PNG) {
        return false;
    }

    std::string database_path = asset_dir + "assets.db";
    result = sqlite3_open_v2(database_path.c_str(), &database, SQLITE_OPEN_READONLY, nullptr);

    if (result) {
        sqlite3_close(database);
        return false;
    }

    result = sqlite3_prepare_v2(database, "SELECT path, hslices, vslices from sprites where assetId = ?;", -1, &select_asset_statement, nullptr);
    if (result)
        return false;

    return true;
}

fw64Texture* fw64Assets::getTexture(int handle) {
    auto existing_texture = textures.find(handle);

    if (existing_texture != textures.end())
        return existing_texture->second.get();

    sqlite3_reset(select_asset_statement);
    sqlite3_bind_int(select_asset_statement, 1, handle);

    if(sqlite3_step(select_asset_statement) != SQLITE_ROW)
        return nullptr;

    std::string sprite_path = reinterpret_cast<const char *>(sqlite3_column_text(select_asset_statement, 0));
    const std::string asset_path = asset_dir + sprite_path;
    auto texture =  fw64Texture::loadImageFile(asset_path);

    texture->hslices = sqlite3_column_int(select_asset_statement, 1);
    texture->vslices = sqlite3_column_int(select_asset_statement, 2);

    textures[handle] = std::unique_ptr<fw64Texture>(texture);

    return texture;
}

int fw64_assets_is_loaded(fw64Assets* assets, uint32_t index) {
    return 0;
}

fw64Mesh* fw64_assets_get_mesh(fw64Assets* assets, uint32_t index) {
    return nullptr;
}

fw64Font* fw64_assets_get_font(fw64Assets* assets, uint32_t index) {
    return nullptr;
}

fw64Texture* fw64_assets_get_image(fw64Assets* assets, uint32_t index) {
    return assets->getTexture(index);
}
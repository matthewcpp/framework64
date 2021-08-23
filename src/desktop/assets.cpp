#include "framework64/assets.h"

#include "framework64/desktop/assets.h"
#include "framework64/desktop/texture.h"

#include <SDL_image.h>

fw64Assets::fw64Assets(std::string const & base_path) {
    asset_dir = base_path + "assets/";
}

bool fw64Assets::init() {
    int result = IMG_Init(IMG_INIT_PNG);

    if ((result & IMG_INIT_PNG) != IMG_INIT_PNG) {
        return false;
    }

    return true;
}

fw64Texture* fw64Assets::getTexture(int handle) {
    // todo: lookup handle path and cache result
    const std::string asset_path = asset_dir + "n64_logo";
    return fw64Texture::loadTexture(asset_path);
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
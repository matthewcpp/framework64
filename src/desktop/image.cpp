#include "framework64/desktop/image.h"

#include "framework64/desktop/asset_database.h"

#include <SDL2/SDL_image.h>

fw64Image* fw64Image::loadFromDatabase(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->select_image_statement);
    sqlite3_bind_int(database->select_image_statement, 1, index);

    if(sqlite3_step(database->select_image_statement) != SQLITE_ROW)
        return nullptr;

    std::string image_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_image_statement, 0));
    const std::string asset_path = database->asset_dir + image_path;
    auto image = fw64Image::loadImageFile(asset_path);

    image->hslices = sqlite3_column_int(database->select_image_statement, 1);
    image->vslices = sqlite3_column_int(database->select_image_statement, 2);

    return image;
}

static fw64Image* createTextureFromSurface(SDL_Surface* surface) {
    auto* texture = new fw64Image();

    texture->width = surface->w;
    texture->height = surface->h;
    texture->hslices = 1;
    texture->vslices = 1;

    GLenum textureFormat;
    if ((surface->format->Rmask & 0xFF) == 0xFF) { // RGB
        textureFormat = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
    }
    else {
        textureFormat = surface->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
    }

    glGenTextures(1, &texture->gl_handle);
    glBindTexture(GL_TEXTURE_2D, texture->gl_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
}

fw64Image* fw64Image::loadImageFile(std::string const& path) {
    auto* surface = IMG_Load(path.c_str());

    if (!surface) {
        return nullptr;
    }

    auto* texture = createTextureFromSurface(surface);
    SDL_FreeSurface(surface);

    return texture;
}

fw64Image* fw64Image::loadImageBuffer(void* data, size_t size) {
    auto* data_stream = SDL_RWFromMem(data, static_cast<int>(size));
    auto* surface = IMG_Load_RW(data_stream, 1);

    if (!surface) {
        return nullptr;
    }

    auto* texture = createTextureFromSurface(surface);
    SDL_FreeSurface(surface);

    return texture;
}

fw64Image::~fw64Image() {
    glDeleteTextures(1, &gl_handle);
}


// C API
fw64Image* fw64_image_load(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    return fw64Image::loadFromDatabase(asset_database, asset_index);
}

fw64Image* fw64_image_load_with_options(fw64AssetDatabase* asset_database, uint32_t asset_index, uint32_t options, fw64Allocator* allocator) {
    return fw64Image::loadFromDatabase(asset_database, asset_index);
}

void fw64_image_delete(fw64AssetDatabase* asset_database, fw64Image* image, fw64Allocator* allocator) {
    delete image;
}
#include "framework64/desktop/image.h"

#include "framework64/desktop/asset_database.h"

#include <SDL2/SDL_image.h>

#include <cassert>

static GLuint createOpenGLTextureFromSurface(SDL_Surface* surface);
static GLuint loadOpenGLTextureFromImageFile(std::string const& path);

fw64Image* fw64Image::loadFromDatabase(fw64AssetDatabase* database, uint32_t index) {
    sqlite3_reset(database->select_image_statement);
    sqlite3_bind_int(database->select_image_statement, 1, index);

    if(sqlite3_step(database->select_image_statement) != SQLITE_ROW)
        return nullptr;

    std::string image_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_image_statement, 0));
    std::string asset_path = database->asset_dir + image_path;
    int indexMode = sqlite3_column_int(database->select_image_statement, 3);

    fw64Image* image = fw64Image::loadImageFile(asset_path, static_cast<bool>(indexMode));

    if (indexMode) {
        sqlite3_reset(database->select_palettes_statement);
        sqlite3_bind_int(database->select_palettes_statement, 1, index);

        while (sqlite3_step(database->select_palettes_statement) == SQLITE_ROW) {
            image_path = reinterpret_cast<const char *>(sqlite3_column_text(database->select_palettes_statement, 0));
            asset_path = database->asset_dir + image_path;
            image->palettes.push_back(loadOpenGLTextureFromImageFile(asset_path));
        }
    } 

    image->hslices = sqlite3_column_int(database->select_image_statement, 1);
    image->vslices = sqlite3_column_int(database->select_image_statement, 2);

    return image;
}

fw64Image::~fw64Image() {
    if (gl_handle != 0)
        glDeleteTextures(1, &gl_handle);

    for (size_t i = 0; i < palettes.size(); i++) {
        glDeleteTextures(1, &palettes[i]);
    }
}

GLuint loadOpenGLTextureFromImageFile(std::string const& path) {
    auto* surface = IMG_Load(path.c_str());

    if (!surface) {
        return 0;
    }

    auto openGlHandle = createOpenGLTextureFromSurface(surface);

    SDL_FreeSurface(surface);

    return openGlHandle;
}

GLuint createOpenGLTextureFromSurface(SDL_Surface* surface) {
    GLuint handle;

    GLenum textureFormat;
    if ((surface->format->Rmask & 0xFF) == 0xFF) { // RGB
        textureFormat = surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
    }
    else {
        textureFormat = surface->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
    }

    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return handle;
}

static fw64Image* createTextureFromSurface(SDL_Surface* surface, bool isIndexedMode) {
    auto gl_handle = createOpenGLTextureFromSurface(surface);;

    auto* texture = new fw64Image();

    texture->width = surface->w;
    texture->height = surface->h;
    texture->hslices = 1;
    texture->vslices = 1;

    if (isIndexedMode) {
        texture->palettes.push_back(gl_handle);
    }
    else {
        texture->gl_handle = gl_handle;
    }

    return texture;
}

fw64Image* fw64Image::loadImageFile(std::string const& path, bool isIndexMode) {
    auto* surface = IMG_Load(path.c_str());

    if (!surface) {
        return nullptr;
    }

    auto* texture = createTextureFromSurface(surface, isIndexMode);
    SDL_FreeSurface(surface);

    return texture;
}

fw64Image* fw64Image::loadImageBuffer(void* data, size_t size, bool isIndexedMode) {
    auto* data_stream = SDL_RWFromMem(data, static_cast<int>(size));
    auto* surface = IMG_Load_RW(data_stream, 1);

    if (!surface) {
        return nullptr;
    }

    auto* texture = createTextureFromSurface(surface, isIndexedMode);
    SDL_FreeSurface(surface);

    return texture;
}

void fw64Image::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    int index = (y * width + x) * 4;
    pixel_data[index] = r;
    pixel_data[index + 1] = g;
    pixel_data[index + 2] = b;
    pixel_data[index + 3] = 255U;
}

void fw64Image::setSize(int w, int h, int h_slices, int v_slices) {
    width = w;
    height = h;
    hslices = h_slices;
    vslices = v_slices;

    pixel_data.resize(width * height * 4);

    if (gl_handle == 0) {
        glGenTextures(1, &gl_handle);
        glBindTexture(GL_TEXTURE_2D, gl_handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        updateGlImage();
    }
}

void fw64Image::updateGlImage() {
    assert(gl_handle != 0 && pixel_data.size() > 0);

    glBindTexture(GL_TEXTURE_2D, gl_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data.data());
}

void fw64Image::clear() {
    std::fill(pixel_data.begin(), pixel_data.end(), 0);
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

void fw64_image_load_frame(fw64Image* image, uint32_t frame) {
    // this is a no-op on desktop
}

uint16_t fw64_image_get_palette_count(fw64Image* image) {
    return static_cast<uint16_t>(image->palettes.size());
}
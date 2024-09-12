#include "framework64/desktop/image.hpp"

#include "framework64/desktop/asset_database.hpp"

#include <SDL2/SDL_image.h>

#include <cassert>
#include <vector>

static GLuint createOpenGLTextureFromSurface(SDL_Surface* surface);
static GLuint loadOpenGLTextureFromBuffer(uint8_t* data, size_t size);

/** This class corresponds to ImageHeader in pipeline/desktop/ImageWriter.js */
struct ImageHeader {
    uint32_t hslices;
    uint32_t vslices;
    uint32_t indexed_mode;
    uint32_t primary_data_size;
    uint32_t additional_palette_count;
};

fw64Image* fw64Image::loadFromDatasource(fw64DataSource* data_source, fw64Allocator*) {
    ImageHeader header;
    fw64_data_source_read(data_source, &header, sizeof(ImageHeader), 1);

    std::vector<uint8_t> image_buffer(header.primary_data_size);
    fw64_data_source_read(data_source, image_buffer.data(), 1, header.primary_data_size);
    auto* image = fw64Image::loadImageBuffer(image_buffer.data(), image_buffer.size(), static_cast<bool>(header.indexed_mode));
    
    image->hslices = header.hslices;
    image->vslices = header.vslices;

    if (header.additional_palette_count == 0) {
        return image;
    }
    
    std::vector<uint32_t> image_sizes(header.additional_palette_count);
    fw64_data_source_read(data_source, image_sizes.data(), sizeof(uint32_t), header.additional_palette_count);
    
    for (uint32_t i = 0; i < header.additional_palette_count; i++) {
        auto const image_data_size = image_sizes[i];
        image_buffer.resize(image_data_size);
        fw64_data_source_read(data_source, image_buffer.data(), 1, image_data_size);
        image->palettes.push_back(loadOpenGLTextureFromBuffer(image_buffer.data(), image_data_size));
    }

    return image;
}

fw64Image::~fw64Image() {
    if (gl_handle != 0)
        glDeleteTextures(1, &gl_handle);

    for (size_t i = 0; i < palettes.size(); i++) {
        glDeleteTextures(1, &palettes[i]);
    }
}

static GLuint loadOpenGLTextureFromBuffer(uint8_t* data, size_t size) {
    auto* data_stream = SDL_RWFromMem(data, static_cast<int>(size));
    auto* surface = IMG_Load_RW(data_stream, 1);

    if (!surface) {
        return 0;
    }   

    auto openGlHandle = createOpenGLTextureFromSurface(surface);

    SDL_FreeSurface(surface);

    return openGlHandle;
}

GLuint createOpenGLTextureFromSurface(SDL_Surface* surface) {
    GLuint handle;

    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    if (surface->format->format != SDL_PIXELFORMAT_RGBA32) {
        SDL_Surface* converted_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, converted_surface->pixels);
        SDL_FreeSurface(converted_surface);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return handle;
}

static fw64Image* createTextureFromSurface(SDL_Surface* surface, bool isIndexedMode) {

    GLuint gl_handle = createOpenGLTextureFromSurface(surface);

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

bool fw64Image::addImagePaletteFromFile(fw64Image* image, std::string const& path) {
    auto* surface = IMG_Load(path.c_str());

    if (!surface) {
        return false;
    }

    GLuint handle = createOpenGLTextureFromSurface(surface);
    image->palettes.push_back(handle);

    return true;
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
fw64Image* fw64_image_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator) {
    assert(allocator != nullptr);
    return fw64Image::loadFromDatasource(data_source, allocator);
}

void fw64_image_delete(fw64AssetDatabase*, fw64Image* image, fw64Allocator*) {
    delete image;
}

void fw64_image_load_frame(fw64Image*, uint32_t) {
    // this is a no-op on desktop
}

uint16_t fw64_image_get_palette_count(fw64Image* image) {
    return static_cast<uint16_t>(image->palettes.size());
}

#include "libdragon_texture.h"

fw64Texture* fw64_texture_create_from_image(fw64Image* image, fw64Allocator* allocator) {
    fw64Texture* texture = fw64_allocator_malloc(allocator, sizeof(fw64Texture));

    texture->image = image;

    return texture;
}
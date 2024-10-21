#include "libdragon_image.h"

void fw64_libdragon_image_init(fw64Image* image, sprite_t* sprite) {
    image->sprite = sprite;
    glGenTextures(1, &image->gl_handle);

    glBindTexture(GL_TEXTURE_2D, image->gl_handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //TODO: does this needs to be moved into the renderer when binding the texture?
    glSpriteTextureN64(GL_TEXTURE_2D, image->sprite, &(rdpq_texparms_t){.s.repeats = REPEAT_INFINITE, .t.repeats = REPEAT_INFINITE});
}

void fw64_libdragon_image_uninit(fw64Image* image, fw64Allocator* allocator) {
    glDeleteTextures(1, &image->gl_handle);
    //fw64_allocator_free(allocator, image->sprite);
    //TODO: this needs to go away when sprite buffer loading is working
    sprite_free(image->sprite);
    fw64_allocator_free(allocator, image);
}

void fw64_image_delete(fw64AssetDatabase* asset_database, fw64Image* image, fw64Allocator* allocator) {
    (void)asset_database;


}

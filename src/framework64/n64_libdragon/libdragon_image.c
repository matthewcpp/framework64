#include "libdragon_image.h"

fw64Image* fw64_image_load_from_datasource(fw64DataSource* data_source, fw64Allocator* allocator) {
    fw64Image* image = fw64_allocator_malloc(allocator, sizeof(fw64Image));

    if (!fw64_libdragon_image_init_from_datasource(image, data_source, allocator)) {
        fw64_allocator_free(allocator, image);
        image = NULL;
    }

    return image;
}

// Note: images are created uncompressed by the pipeline.
// We should be able to load them using the buffer that we read from the data source
int fw64_libdragon_image_init_from_datasource(fw64Image* image, fw64DataSource* datasource, fw64Allocator* allocator) {
    fw64_data_source_read(datasource, &image->info, sizeof(fw64LibdragonImageInfo), 1);

    const uint32_t sprite_count = image->info.hslices * image->info.vslices;
    image->sprites = fw64_allocator_malloc(allocator, sizeof(fw64LibdragonSpriteSlice) * sprite_count);

    for (uint32_t i = 0; i < sprite_count; i++) {
        fw64LibdragonSpriteSlice* slice = image->sprites + i;

        uint32_t sprite_data_size;
        fw64_data_source_read(datasource, &sprite_data_size, sizeof(uint32_t), 1);

        // note this alignment value was taken from internal libdragon
        slice->mem_buffer = fw64_allocator_memalign(allocator, 32, sprite_data_size);
        fw64_data_source_read(datasource, slice->mem_buffer, sizeof(char), sprite_data_size);

        // libdragon will load the sprite in place using the buffer we read from memory
        slice->sprite = sprite_load_buf(slice->mem_buffer, sprite_data_size);

        if (!slice->sprite) {
            fw64_allocator_free(allocator, slice->mem_buffer);
            return 0;
        }

        // register the image for for rendering
        glGenTextures(1, &slice->gl_handle);
        glBindTexture(GL_TEXTURE_2D, slice->gl_handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // TODO: look into REPEAT_INFINITE for rdpq_texparms_t.
        glSpriteTextureN64(GL_TEXTURE_2D, slice->sprite, &(rdpq_texparms_t){.s.repeats = 1.0f, .t.repeats = 1.0f});
    }

    return 1;
}

// note we do not need to free the sprite pointer, it was created in place.
void fw64_libdragon_image_uninit(fw64Image* image, fw64Allocator* allocator) {
    const uint32_t sprite_count = image->info.hslices * image->info.vslices;

    for (uint32_t i = 0; i < sprite_count; i++) {
        fw64LibdragonSpriteSlice* slice = image->sprites + i;

        glDeleteTextures(1, &slice->gl_handle);
        fw64_allocator_free(allocator, slice->mem_buffer);
    }
}

void fw64_image_delete(fw64AssetDatabase* asset_database, fw64Image* image, fw64Allocator* allocator) {
    (void)asset_database;

    fw64_libdragon_image_uninit(image, allocator);
    fw64_allocator_free(allocator, image);
}

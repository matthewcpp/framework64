#include "framework64/n64/asset_database.h"
#include "framework64/n64/filesystem.h"

#include "framework64/animation_data.h"
#include "framework64/scene.h"

#include "framework64/n64/image.h"
#include "framework64/n64/font.h"
#include "framework64/n64/mesh.h"

fw64Image* fw64_assets_load_image(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    fw64N64FilesystemDataSource data_source;
    if (!fw64_n64_filesystem_open_datasource(&data_source, asset_index))
        return NULL;
        
    fw64Image* image = fw64_image_load_from_datasource(&data_source.interface, allocator);
    fw64_n64_filesystem_close_datasource(&data_source);

    return image;
}

fw64Image* fw64_assets_load_image_dma(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    int handle = fw64_filesystem_open(asset_index);
    if (handle < 0)
        return NULL;

    fw64Image* image = allocator->malloc(allocator, sizeof(fw64Image));
    int result = fw64_n64_image_init_dma_mode(image, handle, fw64_n64_filesystem_get_rom_address(asset_index), allocator);

    if (!result)
        allocator->free(allocator, image);

    fw64_filesystem_close(handle);

    return image;
}

fw64Font* fw64_assets_load_font(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    fw64N64FilesystemDataSource data_source;
    if (!fw64_n64_filesystem_open_datasource(&data_source, asset_index))
        return NULL;

    fw64Font* font = fw64_font_load_from_datasource(&data_source.interface, allocator);

    fw64_n64_filesystem_close_datasource(&data_source);

    return font;
}

fw64Mesh* fw64_assets_load_mesh(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    fw64N64FilesystemDataSource data_source;
    if (!fw64_n64_filesystem_open_datasource(&data_source, asset_index))
        return NULL;

    fw64Mesh* mesh = fw64_mesh_load_from_datasource(asset_database, &data_source.interface, allocator);

    fw64_n64_filesystem_close_datasource(&data_source);

    return mesh;
}

fw64Scene* fw64_assets_load_scene(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    fw64N64FilesystemDataSource data_source;
    if (!fw64_n64_filesystem_open_datasource(&data_source, asset_index))
        return NULL;

    fw64Scene* scene = fw64_scene_load_from_datasource(&data_source.interface, asset_database, allocator);

    fw64_n64_filesystem_close_datasource(&data_source);

    return scene;
}

fw64AnimationData* fw64_assets_load_animation_data(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    fw64N64FilesystemDataSource data_source;
    if (!fw64_n64_filesystem_open_datasource(&data_source, asset_index))
        return NULL;

    fw64AnimationData* animation_data = fw64_animation_data_load_from_datasource(&data_source.interface, allocator);

    fw64_n64_filesystem_close_datasource(&data_source);

    return animation_data;
}

fw64SkinnedMesh* fw64_assets_load_skinned_mesh(fw64AssetDatabase* asset_database, uint32_t asset_index, fw64Allocator* allocator) {
    fw64N64FilesystemDataSource data_source;
    if (!fw64_n64_filesystem_open_datasource(&data_source, asset_index))
        return NULL;

    fw64SkinnedMesh* skinned_mesh = fw64_skinned_mesh_load_from_datasource(&data_source.interface, asset_database, allocator);

    fw64_n64_filesystem_close_datasource(&data_source);

    return skinned_mesh;
}
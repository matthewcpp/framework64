#include "framework64/n64/asset_database.h"

#include "framework64/animation_data.h"
#include "framework64/scene.h"

#include "framework64/n64/image.h"
#include "framework64/n64/font.h"
#include "framework64/n64/mesh.h"

void fw64_n64_asset_database_init(fw64AssetDatabase* assets) {
    for (size_t i = 0; i < FW64_ASSETS_MAX_OPEN_DATASOURCES; i++) {
        assets->data_sources[i].file_handle = FW64_FILESYSTEM_INVALID_HANDLE;
    }
}

fw64DataSource* fw64_assets_open_datasource(fw64AssetDatabase* assets, fw64AssetId asset_id) {
    for (size_t i = 0; i < FW64_ASSETS_MAX_OPEN_DATASOURCES; i++) {
        fw64N64FilesystemDataSource* data_source = assets->data_sources + i;
        if (data_source->file_handle == FW64_FILESYSTEM_INVALID_HANDLE && fw64_n64_filesystem_open_datasource(data_source, asset_id)) {
            return &data_source->interface;
        }
    }
    return NULL;
}

void fw64_assets_close_datasource(fw64AssetDatabase* asset_database, fw64DataSource* datasource) {
    fw64N64FilesystemDataSource* filesystem_data_source = (fw64N64FilesystemDataSource*)datasource;
    fw64_n64_filesystem_close_datasource(filesystem_data_source);
}

fw64Image* fw64_assets_load_image(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64DataSource* data_source = fw64_assets_open_datasource(asset_database, asset_id);
    if (!data_source)
        return NULL;
        
    fw64Image* image = fw64_image_load_from_datasource(data_source, allocator);
    fw64_assets_close_datasource(asset_database, data_source);

    return image;
}

fw64Image* fw64_assets_load_image_dma(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    int handle = fw64_filesystem_open(asset_id);
    if (handle < 0)
        return NULL;

    fw64Image* image = allocator->malloc(allocator, sizeof(fw64Image));
    int result = fw64_n64_image_init_dma_mode(image, handle, fw64_n64_filesystem_get_rom_address(asset_id), allocator);

    if (!result)
        allocator->free(allocator, image);

    fw64_filesystem_close(handle);

    return image;
}

fw64Font* fw64_assets_load_font(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64DataSource* data_source = fw64_assets_open_datasource(asset_database, asset_id);
    if (!data_source)
        return NULL;

    fw64Font* font = fw64_font_load_from_datasource(data_source, allocator);

    fw64_assets_close_datasource(asset_database, data_source);

    return font;
}

fw64Mesh* fw64_assets_load_mesh(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64DataSource* data_source = fw64_assets_open_datasource(asset_database, asset_id);
    if (!data_source)
        return NULL;

    fw64Mesh* mesh = fw64_mesh_load_from_datasource(asset_database, data_source, allocator);

    fw64_assets_close_datasource(asset_database, data_source);

    return mesh;
}

fw64Scene* fw64_assets_load_scene(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64DataSource* data_source = fw64_assets_open_datasource(asset_database, asset_id);
    if (!data_source)
        return NULL;

    fw64Scene* scene = fw64_scene_load_from_datasource(data_source, asset_database, allocator);

    fw64_assets_close_datasource(asset_database, data_source);

    return scene;
}

fw64AnimationData* fw64_assets_load_animation_data(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64DataSource* data_source = fw64_assets_open_datasource(asset_database, asset_id);
    if (!data_source)
        return NULL;

    fw64AnimationData* animation_data = fw64_animation_data_load_from_datasource(data_source, allocator);

    fw64_assets_close_datasource(asset_database, data_source);

    return animation_data;
}

fw64SkinnedMesh* fw64_assets_load_skinned_mesh(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64DataSource* data_source = fw64_assets_open_datasource(asset_database, asset_id);
    if (!data_source)
        return NULL;

    fw64SkinnedMesh* skinned_mesh = fw64_skinned_mesh_load_from_datasource(data_source, asset_database, allocator);

    fw64_assets_close_datasource(asset_database, data_source);

    return skinned_mesh;
}

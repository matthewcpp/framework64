#include "libdragon_asset_database.h"

#include "libdragon_font.h"
#include "libdragon_image.h"
#include "libdragon_mesh.h"

#include "string.h"

void fw64_libdragon_asset_database_init(fw64AssetDatabase* asset_database) {
    // initialize datasources to unopened;
    memset(asset_database, 0, sizeof(fw64AssetDatabase));
}

void fw64_libdragon_asset_database_filepath(fw64AssetId asset_id, char* buffer) {
    sprintf(buffer, "rom://%u", asset_id);
}

// This needs to be kept in sync with _assetBundleFileName in DfsAssets.js
#define ASSET_BUNDLE_PATH "rom:/assets.bundle"

fw64DataSource* fw64_assets_open_datasource(fw64AssetDatabase* asset_database, fw64AssetId asset_id) {
    size_t offset = (size_t)(asset_id >> 32); // high32 bits
    size_t size  = (size_t)(asset_id & 0xFFFFFFFFu); // low 32 bits

    for (size_t i = 0; i < DATASOURCE_COUNT; i++) {
        fw64DfsDataSource* datasource = &asset_database->datasources[i];
        if (fw64_dfs_datasource_open(datasource, ASSET_BUNDLE_PATH, offset, size)) {
            return &datasource->interface;
        }
    }

    return NULL;
}

void fw64_assets_close_datasource(fw64AssetDatabase* asset_database, fw64DataSource* datasource) {
    (void)asset_database;
    fw64_dfs_datasource_close((fw64DfsDataSource*)datasource);
}

fw64Image* fw64_assets_load_image(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64DataSource* datasource = fw64_assets_open_datasource(asset_database, asset_id);

    if (!datasource) {
        return NULL;
    }

    fw64Image* image = fw64_image_load_from_datasource(datasource, allocator);

    fw64_assets_close_datasource(asset_database, datasource);

    return image;
}

fw64Font* fw64_assets_load_font(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64DataSource* datasource = fw64_assets_open_datasource(asset_database, asset_id);

    if (!datasource) {
        return NULL;
    }

    fw64Font* font = fw64_font_load_from_datasource(datasource, allocator);

    fw64_assets_close_datasource(asset_database, datasource);

    return font;
}

fw64Mesh* fw64_assets_load_mesh(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64DataSource* datasource = fw64_assets_open_datasource(asset_database, asset_id);
    if (!datasource) {
        return NULL;
    }

    fw64Mesh* mesh = fw64_mesh_load_from_datasource(asset_database, datasource, allocator);
    fw64_assets_close_datasource(asset_database, datasource);
    return mesh;
}

fw64SkinnedMesh* fw64_assets_load_skinned_mesh(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    fw64DataSource* datasource = fw64_assets_open_datasource(asset_database, asset_id);
    if (!datasource) {
        return NULL;
    }

    fw64SkinnedMesh* skinned_mesh = fw64_skinned_mesh_load_from_datasource(datasource, asset_database, allocator);
    fw64_assets_close_datasource(asset_database, datasource);
    return skinned_mesh;
}

#if 0
#include <sys/stat.h>
#include <unistd.h>

extern FILE *must_fopen(const char *fn);
extern int must_open(const char *fn);

#define ASSET_ALIGNMENT 32

fw64Image* fw64_assets_load_image(fw64AssetDatabase* asset_database, fw64AssetId asset_id, fw64Allocator* allocator) {
    (void)asset_database;

    fw64Image* image = fw64_allocator_malloc(allocator, sizeof(fw64Image));

    // TODO: write this data out at asset preop time
    int f = must_open(asset_id);
    struct stat stat;
    fstat(f, &stat);
    int file_size = stat.st_size, buffer_size = 0;
    close(f);

    FILE* fd = must_fopen(asset_id);
    asset_loadf_into(fd, &file_size, NULL, &buffer_size);
    void* asset_buffer = fw64_allocator_memalign(allocator, ASSET_ALIGNMENT, (size_t)buffer_size);
    fseek(fd, 0, SEEK_SET);
    asset_loadf_into(fd, &file_size, asset_buffer, &buffer_size);
    image->libdragon_sprite = (sprite_t*)asset_buffer;
    fclose(fd);
    
    return image;
}
#endif
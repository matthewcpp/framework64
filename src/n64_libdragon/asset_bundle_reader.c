#include "framework64/n64_libdragon/asset_bundle_reader.h"

#include <string.h>

#include <libdragon.h>

void fw64_asset_bundle_reader_init(fw64AssetBundleReader* bundle_reader) {
    memset(bundle_reader, 0, sizeof(fw64AssetBundleReader));
    bundle_reader->datasource.read = fw64_asset_bundle_data_source_read;
    bundle_reader->datasource.size = fw64_asset_bundle_data_source_size;
}

int fw64_asset_bundle_reader_open(fw64AssetBundleReader* bundle_reader, const char* path) {
    if (bundle_reader->file_handle) {
        fw64_asset_bundle_reader_close(bundle_reader);
    }

    bundle_reader->file_handle = fopen(path, "r");
    if (!bundle_reader->file_handle) {
        return 0;
    }

    return 1;
}

int fw64_asset_bundle_reader_reset(fw64AssetBundleReader* bundle_reader, uint64_t key) {
    long offset = (long)(key >> 32);

    bundle_reader->data_read = 0;
    bundle_reader->size = (uint32_t)(key & 0x00000000FFFFFFFF);

    debugf("asset bundle open: %d, %d\n", (int)offset, (int)bundle_reader->size);

    fseek(bundle_reader->file_handle, offset, SEEK_SET);

    return 1;
}

void fw64_asset_bundle_reader_close(fw64AssetBundleReader* data_source) {
    fclose(data_source->file_handle);
}

size_t fw64_asset_bundle_data_source_size(fw64DataSource* data_source) {
    fw64AssetBundleReader* bundle_reader = (fw64AssetBundleReader*)data_source;

    return bundle_reader->size;
}

size_t fw64_asset_bundle_data_source_read(fw64DataSource* data_source, void* buffer, size_t size, size_t count) {
    fw64AssetBundleReader* bundle_reader = (fw64AssetBundleReader*)data_source;

    size_t data_requested = size * count;
    size_t data_available = bundle_reader->size - bundle_reader->data_read;
    size_t data_to_read = data_requested <= data_available ? data_requested : data_available;

    fread(buffer, 1, data_to_read, bundle_reader->file_handle);
    bundle_reader->data_read += data_to_read;

    return data_to_read;
}

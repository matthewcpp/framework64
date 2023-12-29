#pragma once

#include "framework64/data_io.h"

#include <stdint.h>
#include <stdio.h>

typedef struct {
    fw64DataSource datasource;
    uint32_t data_read;
    uint32_t size;
    FILE* file_handle;
} fw64AssetBundleReader;

#ifdef __cplusplus
extern "C" {
#endif

void fw64_asset_bundle_reader_init(fw64AssetBundleReader* bundle_reader);
int fw64_asset_bundle_reader_open(fw64AssetBundleReader* bundle_reader, const char* path);
void fw64_asset_bundle_reader_close(fw64AssetBundleReader* bundle_reader);
int fw64_asset_bundle_reader_reset(fw64AssetBundleReader* bundle_reader, uint64_t key);

size_t fw64_asset_bundle_data_source_size(fw64DataSource* data_source);
size_t fw64_asset_bundle_data_source_read(fw64DataSource* data_source, void* buffer, size_t size, size_t count);

#ifdef __cplusplus
}
#endif
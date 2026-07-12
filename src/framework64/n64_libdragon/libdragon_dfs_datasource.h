#pragma once

#include "framework64/data_io.h"

#include <stdint.h>
#include <stdio.h>

typedef struct {
    fw64DataSource interface;
    size_t offset;
    size_t size;
    size_t cursor_pos;
    FILE* bundle_file;
} fw64DfsDataSource;


/** Opens the asset with offset / size info for the asset. */
int fw64_dfs_datasource_open(fw64DfsDataSource* datasource, const char* bundle_path, size_t offset, size_t size);

/** Closes the data source, making it available for reuse later*/
int fw64_dfs_datasource_close(fw64DfsDataSource* datasource);

/** Reads from the current cursor position and returns the number of bytes read */
size_t fw64_dfs_datasource_read(fw64DataSource* data_source, void* buffer, size_t size, size_t count);

/** Simply return the size that this data source was initialized with */
size_t fw64_dfs_datasource_size(fw64DataSource* data_source);

/** Seeks from the offset position */
int fw64_dfs_datasource_seek(fw64DataSource* data_source, size_t offset);
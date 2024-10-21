#pragma once

#include "framework64/data_io.h"

#include <stdio.h>

typedef struct {
    fw64DataSource interface;
    size_t size;
    FILE* dfs_file;
} fw64DfsDataSource;

int fw64_dfs_datasource_open(fw64DfsDataSource* datasource, const char* path);
int fw64_dfs_datasource_close(fw64DfsDataSource* datasource);
size_t fw64_dfs_datasource_read(fw64DataSource* data_source, void* buffer, size_t size, size_t count);
size_t fw64_dfs_datasource_size(fw64DataSource* data_source);
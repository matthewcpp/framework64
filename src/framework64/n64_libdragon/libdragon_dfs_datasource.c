#include "libdragon_dfs_datasource.h"

int fw64_dfs_datasource_open(fw64DfsDataSource* datasource, const char* path) {
    if (datasource->dfs_file) {
        return 0;
    }

    datasource->dfs_file = fopen(path, "rb");

    if (!datasource->dfs_file) {
        return 0;
    }

    fseek(datasource->dfs_file, 0, SEEK_END);
    datasource->size = ftell(datasource->dfs_file);
    fseek(datasource->dfs_file, 0, SEEK_SET);

    datasource->interface.read = fw64_dfs_datasource_read;
    datasource->interface.size = fw64_dfs_datasource_size;

    return 1;
}

int fw64_dfs_datasource_close(fw64DfsDataSource* datasource) {
    if (datasource->dfs_file) {
        fclose(datasource->dfs_file);
        datasource->dfs_file = NULL;
        return 1;
    }

    return 0;
}

size_t fw64_dfs_datasource_read(fw64DataSource* data_source, void* buffer, size_t size, size_t count) {
    return fread(buffer, size, count, ((fw64DfsDataSource*)data_source)->dfs_file);
}

size_t fw64_dfs_datasource_size(fw64DataSource* data_source) {
    return ((fw64DfsDataSource*)data_source)->size;
}
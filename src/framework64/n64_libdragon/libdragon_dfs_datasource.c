#include "libdragon_dfs_datasource.h"

int fw64_dfs_datasource_open(fw64DfsDataSource* datasource, const char* bundle_path, size_t offset, size_t size) {
    if (datasource->bundle_file) {
        return 0;
    }

    datasource->bundle_file = fopen(bundle_path, "r");

    if (!datasource->bundle_file) {
        return 0;
    }

    datasource->cursor_pos = 0;
    datasource->offset = offset;
    datasource->size = size;

    fseek(datasource->bundle_file, datasource->offset, SEEK_SET);

    datasource->interface.read = fw64_dfs_datasource_read;
    datasource->interface.size = fw64_dfs_datasource_size;
    datasource->interface.seek = fw64_dfs_datasource_seek;

    return 1;
}

int fw64_dfs_datasource_close(fw64DfsDataSource* datasource) {
    if (datasource->bundle_file) {
        fclose(datasource->bundle_file);
        datasource->bundle_file = NULL;
        return 1;
    }

    return 0;
}

size_t fw64_dfs_datasource_read(fw64DataSource* data_source, void* buffer, size_t size, size_t count) {
    fw64DfsDataSource* dfs_datasource = (fw64DfsDataSource*)data_source;
    size_t bytes_to_read = size * count;
    size_t bytes_available = dfs_datasource->size - dfs_datasource->cursor_pos;
    if (bytes_to_read > bytes_available) {
        bytes_to_read = bytes_available;
    }

    size_t total_bytes_read = fread(buffer, 1, bytes_to_read, dfs_datasource->bundle_file);
    dfs_datasource->cursor_pos += total_bytes_read;

    return total_bytes_read;
}

size_t fw64_dfs_datasource_size(fw64DataSource* data_source) {
    return ((fw64DfsDataSource*)data_source)->size;
}

int fw64_dfs_datasource_seek(fw64DataSource* data_source, size_t offset) {
    fw64DfsDataSource* dfs_datasource = (fw64DfsDataSource*)data_source;

    if (offset > dfs_datasource->size) {
        offset = dfs_datasource->size;
    }

    size_t new_pos = dfs_datasource->offset + offset;
    return !fseek(dfs_datasource->bundle_file, dfs_datasource->offset + offset, SEEK_SET);
}

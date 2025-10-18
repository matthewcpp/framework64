#include "framework64/data_io.h"

size_t fw64_data_source_size(fw64DataSource* data_source) {
    return data_source->size(data_source);
}

size_t fw64_data_source_read(fw64DataSource* data_source, void* buffer, size_t size, size_t count) {
    return data_source->read(data_source, buffer, size, count);
}

int fw64_data_source_seek(fw64DataSource* data_source, size_t offset) {
    return data_source->seek(data_source, offset);
}

size_t fw64_data_writer_write(fw64DataWriter* data_writer, const char* buffer, size_t size, size_t count) {
    return data_writer->write(data_writer, buffer, size, count);
}

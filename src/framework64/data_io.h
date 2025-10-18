#pragma once

/** \file data_io.h */

#include <stdint.h>
#include <stdlib.h>

typedef struct fw64DataSource fw64DataSource;

typedef size_t(*DataSourceSizeFunc)(fw64DataSource* data_source);
typedef size_t(*DataSourceReadFunc)(fw64DataSource* data_source, void* buffer, size_t size, size_t count);
typedef int (*DataSourceSeekFunc)(fw64DataSource* data_source, size_t offset);

struct fw64DataSource {
    DataSourceSizeFunc size;
    DataSourceReadFunc read;
    DataSourceSeekFunc seek;
};

typedef struct fw64DataWriter fw64DataWriter;

typedef size_t(*DataWriterWriteFunc)(fw64DataWriter* data_source, const char* buffer, size_t size, size_t count);

struct fw64DataWriter {
    DataWriterWriteFunc write;
};

#ifdef __cplusplus
extern "C" {
#endif

size_t fw64_data_source_size(fw64DataSource* data_source);
size_t fw64_data_source_read(fw64DataSource* data_source, void* buffer, size_t size, size_t count);

/** Attempts to seek the datasource to the supplied offset from the beginning of the stream.
 * A nonzero return value indicates that the operation succeded.
 */
int fw64_data_source_seek(fw64DataSource* data_source, size_t offset);

#define fw64_data_source_can_seek(data_source) ((data_source)->seek != NULL)

size_t fw64_data_writer_write(fw64DataWriter* data_writer, const char* buffer, size_t size, size_t count);

#ifdef __cplusplus
}
#endif

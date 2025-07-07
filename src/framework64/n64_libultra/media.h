#pragma once

#include "framework64/media.h"
#include "framework64/data_io.h"

#include "ff.h"

struct fw64MediaDirItr {
    DIR dir;
    FILINFO fno;
    FRESULT status;
};

typedef struct {
    fw64DataSource interface;
    FIL file;
} fw64N64MediaDataReader;

void fw64_n64_media_data_reader_init(fw64N64MediaDataReader* reader);

typedef struct {
    fw64DataWriter interface;
    FIL file;
} fw64N64MediaDataWriter;

void fw64_n64_media_data_writer_init(fw64N64MediaDataWriter* writer);

struct fw64Media {
    int is_present;
    FATFS fs;

    // temporary just to see if this is going to work.. need to manage arrays of these in the future
    fw64MediaDirItr itr;
    fw64N64MediaDataReader data_reader;
    fw64N64MediaDataWriter data_writer;
};

#include "framework64/n64_libultra/media.h"

// reference for Filesystem module: http://elm-chan.org/fsw/ff/00index_e.html

#ifdef FW64_MEDIA_ENABLED

#include "cart.h"

void fw64_n64_media_init(fw64Media* media) {
    int error = cart_init();

    if (error != 0) {
        media->is_present = 0;
        return;
    }

    FRESULT result = f_mount(&media->fs, "", 0);
    media->is_present = (result == FR_OK);

    fw64_n64_media_data_reader_init(&media->data_reader);
    fw64_n64_media_data_writer_init(&media->data_writer);
}

int fw64_media_is_present(fw64Media* media) {
    return media->is_present;
}

fw64MediaDirItr* fw64_media_open_dir(fw64Media* media, const char* path) {
    FRESULT result = f_opendir(&media->itr.dir, path);

    if(result == FR_OK)
        return &media->itr;
    else
        return NULL;
}

void fw64_media_close_dir(fw64Media* media, fw64MediaDirItr* itr) {
    f_closedir(&media->itr.dir);
}

int fw64_media_dir_itr_next(fw64MediaDirItr* itr) {
    itr->status = f_readdir(&itr->dir, &itr->fno);

    if (itr->status != FR_OK || itr->fno.fname[0] == 0)
        return 0;
    
    return 1;
}

const char* fw64_media_dir_itr_name(fw64MediaDirItr* itr) {
    if (itr->status == FR_OK)
        return itr->fno.fname;
    else
        return NULL;
}

fw64DataSource* fw64_media_open_data_source(fw64Media* media, const char* path) {
    FRESULT result = f_open(&media->data_reader.file, path, FA_READ);

    return result == FR_OK ? &media->data_reader.interface : NULL;
}

void fw64_media_close_data_source(fw64Media* media, fw64DataSource* data_source) {
    f_close(&media->data_reader.file);
}

fw64DataWriter* fw64_media_open_data_writer(fw64Media* media, const char* path) {
    FRESULT result = f_open(&media->data_writer.file, path, FA_WRITE | FA_CREATE_ALWAYS);

    return result == FR_OK ? &media->data_writer.interface : NULL;
}

void fw64_media_close_data_writer(fw64Media* media, fw64DataWriter* data_writer) {
    if (data_writer == NULL)
        return;

    f_close(&media->data_writer.file);
}

fw64MediaItemType fw64_media_get_path_type(fw64Media* media, const char* path) {
    FRESULT result;
    FILINFO file_info;

    result = f_stat(path, &file_info);

    if (result != FR_OK)
        return FW64_MEDIA_ITEM_NONE;

    if (file_info.fattrib & AM_DIR)
        return FW64_MEDIA_ITEM_DIRECTORY;
    else
        return FW64_MEDIA_ITEM_FILE;
}

fw64MediaItemType fw64_media_dir_itr_type(fw64MediaDirItr* itr) {
    if (itr->fno.fattrib & AM_DIR)
        return FW64_MEDIA_ITEM_DIRECTORY;
    else 
        return FW64_MEDIA_ITEM_FILE;
}

int fw64_media_create_directory(fw64Media* media, const char* path) {
    FRESULT result = f_mkdir(path);

    return result == FR_OK;
}

int fw64_media_remove(fw64Media* media, const char* path) {
    FRESULT result = f_unlink(path);

    return result == FR_OK;
}

static size_t fw64_n64_media_data_reader_read(fw64DataSource* interface, void* buffer, size_t size, size_t count) {
    fw64N64MediaDataReader* data_reader = (fw64N64MediaDataReader*)interface;

    size_t size_to_read = size * count;
    UINT size_read = 0;

    // todo: capture error?
    f_read(&data_reader->file, buffer, (UINT)size_to_read, &size_read);
    
    return (size_t)size_read;
}

static size_t fw64_n64_media_data_reader_size(fw64DataSource* interface) {
    fw64N64MediaDataReader* data_reader = (fw64N64MediaDataReader*)interface;
    return (size_t)f_size(&data_reader->file);
}

void fw64_n64_media_data_reader_init(fw64N64MediaDataReader* reader) {
    reader->interface.read = fw64_n64_media_data_reader_read;
    reader->interface.size = fw64_n64_media_data_reader_size;
}

static size_t fw64_n64_media_data_writer_write(fw64DataWriter* interface, const char* buffer, size_t size, size_t count) {
    fw64N64MediaDataWriter* data_writer = (fw64N64MediaDataWriter*)interface;
    UINT bytes_to_write = size * count;
    UINT bytes_written = 0;

    f_write(&data_writer->file, buffer, bytes_to_write, &bytes_written);

    return bytes_written;
}

void fw64_n64_media_data_writer_init(fw64N64MediaDataWriter* writer) {
    writer->interface.write = fw64_n64_media_data_writer_write;
}

#else

void fw64_n64_media_init(fw64Media* media) {
    media->is_present = 0;
}

int fw64_media_is_present(fw64Media* media) {
    return 0;
}

fw64MediaDirItr* fw64_media_open_dir(fw64Media* media, const char* path) {
    return NULL;
}

void fw64_media_close_dir(fw64Media* media, fw64MediaDirItr* itr) {

}

int fw64_media_dir_itr_next(fw64MediaDirItr* itr) {
    return 0;
}

const char* fw64_media_dir_itr_name(fw64MediaDirItr* itr) {
    return NULL;
}

fw64DataSource* fw64_media_open_data_source(fw64Media* media, const char* path) {
    return NULL;
}

void fw64_media_close_data_source(fw64Media* media, fw64DataSource* data_source) {

}

fw64DataWriter* fw64_media_open_data_writer(fw64Media* media, const char* path) {
    return NULL;
}

void fw64_media_close_data_writer(fw64Media* media, fw64DataWriter* data_writer) {

}

fw64MediaItemType fw64_media_get_path_type(fw64Media* media, const char* path) {
    return FW64_MEDIA_ITEM_NONE;
}

fw64MediaItemType fw64_media_dir_itr_type(fw64MediaDirItr* itr) {
    return FW64_MEDIA_ITEM_NONE;
}

int fw64_media_create_directory(fw64Media* media, const char* path) {
    return 0;
}

int fw64_media_remove(fw64Media* media, const char* path) {
    return 0;
}

#endif

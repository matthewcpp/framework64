#include "file_downloader.h"

#include "framework64/log.h"

#include <stdio.h>
#include <string.h>

#include "framework64/asset_database.h"
#include "file_downloader_message.h"

#define BUMP_ALLOCATOR_SIZE 256

#define FW64_ROOT_MEDIA_DIR "/framework64"
#define FW64_DATA_LINK_EXAMPLE_DIR "/framework64/data_link"

static void fw64_file_downloader_init_media_dir(fw64FileDownloader* file_downloader);
static void fw64_file_downloader_begin(fw64FileDownloader* file_downloader, fw64DataSource* message);
static void fw64_file_downloader_continue(fw64FileDownloader* file_downloader, fw64DataSource* message);

void fw64_file_downloader_init(fw64FileDownloader* file_downloader, fw64Engine* engine) {
    memset(file_downloader, 0, sizeof(fw64FileDownloader));
    file_downloader->engine = engine;
    fw64_file_downloader_init_media_dir(file_downloader);
    fw64_bump_allocator_init(&file_downloader->bump_allocator, BUMP_ALLOCATOR_SIZE);
}

void fw64_file_downloader_uninit(fw64FileDownloader* file_downloader) {
    if (file_downloader->data_writer) {
        fw64_media_close_data_writer(file_downloader->engine->media, file_downloader->data_writer);
    }
}

uint32_t fw64_file_downloader_receive_message(fw64FileDownloader* file_downloader, fw64DataSource* message) {
    uint32_t message_type;
    fw64_data_source_read(message, &message_type, sizeof(uint32_t), 1);

    if (message_type == Fw64_FILE_DOWNLOADER_MESSAGE_BEGIN) {
        fw64_file_downloader_begin(file_downloader, message);
        return Fw64_FILE_DOWNLOADER_MESSAGE_BEGIN;
    }
    else if (message_type == Fw64_FILE_DOWNLOADER_MESSAGE_DATA) {
        fw64_file_downloader_continue(file_downloader, message);
        return Fw64_FILE_DOWNLOADER_MESSAGE_DATA;
    }

    return Fw64_FILE_DOWNLOADER_MESSAGE_UNKNOWN;
}

typedef struct {
    uint32_t file_name_size;
    uint32_t file_data_size;
} DownloadBeginMessageHeader;

void fw64_file_downloader_begin(fw64FileDownloader* file_downloader, fw64DataSource* message) {
    fw64_bump_allocator_reset(&file_downloader->bump_allocator);

    DownloadBeginMessageHeader message_header;
    fw64_data_source_read(message, &message_header, sizeof(message_header), 1);

    // read asset filename
    file_downloader->current_asset_filename = fw64_bump_allocator_malloc(&file_downloader->bump_allocator, message_header.file_name_size + 1);
    fw64_data_source_read(message, file_downloader->current_asset_filename, 1, message_header.file_name_size);
    file_downloader->current_asset_filename[message_header.file_name_size] = 0;

    // construct asset filepath
    size_t filepath_size = strlen(FW64_DATA_LINK_EXAMPLE_DIR) + 1 /* / */ + message_header.file_name_size + 1;
    file_downloader->current_asset_filepath = fw64_bump_allocator_malloc(&file_downloader->bump_allocator, filepath_size);
    sprintf(file_downloader->current_asset_filepath, "%s/%s", FW64_DATA_LINK_EXAMPLE_DIR, file_downloader->current_asset_filename);
    
    // prepare to read data
    file_downloader->current_asset_size = message_header.file_data_size;
    file_downloader->current_asset_received = 0;

    file_downloader->data_writer = fw64_media_open_data_writer(file_downloader->engine->media, &file_downloader->current_asset_filepath[0]);

    fw64_debug_log("Saving file: %s", &file_downloader->current_asset_filepath[0]);

    if (file_downloader->callbacks.begin) {
        file_downloader->callbacks.begin(file_downloader, file_downloader->callbacks_arg);
    }
}

#define MESSAGE_CHUNK_BUFFER_SIZE 1024

void fw64_file_downloader_continue(fw64FileDownloader* file_downloader, fw64DataSource* message) {
    uint32_t payload_size;
    fw64_data_source_read(message, &payload_size, sizeof(uint32_t), 1);
    uint32_t payload_data_remaining = payload_size;
    char message_chunk_buffer[MESSAGE_CHUNK_BUFFER_SIZE];

    while (payload_data_remaining > 0) {
        size_t data_to_read = payload_data_remaining > MESSAGE_CHUNK_BUFFER_SIZE ? MESSAGE_CHUNK_BUFFER_SIZE : payload_data_remaining;
        fw64_data_source_read(message, &message_chunk_buffer[0], 1, data_to_read);

        if (file_downloader->data_writer) {
            fw64_data_writer_write(file_downloader->data_writer, &message_chunk_buffer[0], 1, data_to_read);
        }
        payload_data_remaining -= data_to_read;
        file_downloader->current_asset_received += data_to_read;
    }

    if (file_downloader->current_asset_received == file_downloader->current_asset_size) {
        fw64_media_close_data_writer(file_downloader->engine->media, file_downloader->data_writer);
        file_downloader->data_writer = NULL;

        if (file_downloader->callbacks.complete) {
            file_downloader->callbacks.complete(file_downloader, file_downloader->callbacks_arg);
        }
    }
    else if(file_downloader->callbacks.progress) {
        file_downloader->callbacks.progress(file_downloader, file_downloader->callbacks_arg);
    }
}

void fw64_file_downloader_init_media_dir(fw64FileDownloader* file_downloader) {
    if (fw64_media_get_path_type(file_downloader->engine->media, FW64_ROOT_MEDIA_DIR) == FW64_MEDIA_ITEM_NONE) {
        fw64_media_create_directory(file_downloader->engine->media, FW64_ROOT_MEDIA_DIR);
    }

    if (fw64_media_get_path_type(file_downloader->engine->media, FW64_DATA_LINK_EXAMPLE_DIR) == FW64_MEDIA_ITEM_NONE) {
        fw64_media_create_directory(file_downloader->engine->media, FW64_DATA_LINK_EXAMPLE_DIR);
    }
}

void fw64_file_downloader_set_callbacks(fw64FileDownloader* file_downloader, fw64FileDownloaderCallbacks* callbacks, void* arg) {
    memcpy(&file_downloader->callbacks, callbacks, sizeof(fw64FileDownloaderCallbacks));
    file_downloader->callbacks_arg = arg;
}

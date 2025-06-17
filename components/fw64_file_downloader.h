#include "framework64/engine.h"
#include "framework64/media.h"
#include "framework64/util/bump_allocator.h"

typedef struct fw64FileDownloader fw64FileDownloader;

typedef void (*fw64FileDownloaderBeginCallback)(fw64FileDownloader* file_downloader, void* arg);
typedef void (*fw64FileDownloaderProgressCallback)(fw64FileDownloader* file_downloader, void* arg);
typedef void (*fw64FileDownloaderCompleteCallback)(fw64FileDownloader* file_downloader, void* arg);

typedef struct {
    fw64FileDownloaderBeginCallback begin;
    fw64FileDownloaderProgressCallback progress;
    fw64FileDownloaderCompleteCallback complete;
} fw64FileDownloaderCallbacks;

struct fw64FileDownloader{
    fw64Engine* engine;
    fw64Media* media;
    fw64DataWriter* data_writer;
    fw64BumpAllocator bump_allocator;
    char* current_asset_filename;
    char* current_asset_filepath;
    uint32_t current_asset_size;
    uint32_t current_asset_received;
    fw64FileDownloaderCallbacks callbacks;
    void* callbacks_arg;
};

#ifdef __cplusplus
extern "C" {
#endif

void fw64_file_downloader_init(fw64FileDownloader* file_downloader, fw64Engine* engine);
void fw64_file_downloader_uninit(fw64FileDownloader* file_downloader);
uint32_t fw64_file_downloader_receive_message(fw64FileDownloader* file_downloader, fw64DataSource* message);
void fw64_file_downloader_set_callbacks(fw64FileDownloader* file_downloader, fw64FileDownloaderCallbacks* callbacks, void* arg);

#ifdef __cplusplus
}
#endif

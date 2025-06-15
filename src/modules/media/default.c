#include "modules/media.h"

struct fw64Media {
    int is_present;
};

static fw64Media default_media;

fw64Media* _fw64_media_init(void* param) {
    default_media.is_present = 0;
    return &default_media;
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

fw64MediaItemType fw64_media_dir_itr_type(fw64MediaDirItr* itr) {
    return FW64_MEDIA_ITEM_NONE;
}

fw64MediaItemType fw64_media_get_path_type(fw64Media* media, const char* path) {
    return FW64_MEDIA_ITEM_NONE;
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

int fw64_media_create_directory(fw64Media* media, const char* path) {
    return 0;
}

int fw64_media_remove(fw64Media* media, const char* path) {
    return 0;
}

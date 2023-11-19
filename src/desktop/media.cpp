#include "framework64/desktop/media.h"

#include <iostream>

bool fw64Media::init(std::string const & media_dir) {
    base_path = media_dir;

    if (!std::filesystem::is_directory(media_dir)) {
        bool result = std::filesystem::create_directory(media_dir);

        if (!result) {
            std::cout << "Failed to create media directory: " << media_dir << std::endl;
        }
    }

    std::cout << "Using media dir: " << media_dir << std::endl;

    return true;
}

// C-API
int fw64_media_is_present(fw64Media* media) {
    return !media->base_path.empty();
}

fw64MediaDirItr* fw64_media_open_dir(fw64Media* media, const char* path) {
    if (!path || path[0] == 0)
        return nullptr;

    // currently only support one directory iterator
    if (!media->dir_itr.path.empty())
        return nullptr;

    auto directory_path = media->base_path / (path + 1);

    if (!std::filesystem::is_directory(directory_path))
        return nullptr;

    media->dir_itr.path = directory_path.string();
    media->dir_itr.dir_itr = std::filesystem::directory_iterator{directory_path};

    return &media->dir_itr;
}

void fw64_media_close_dir(fw64Media* media, fw64MediaDirItr* itr) {
    media->dir_itr.path.clear();
}

int fw64_media_dir_itr_next(fw64MediaDirItr* itr) {
    if (itr->dir_itr == std::filesystem::end(itr->dir_itr)) {
        itr->current_item_name.clear();
        itr->current_item_type = FW64_MEDIA_ITEM_NONE;
        return 0;
    }

    auto const & dir_entry = *itr->dir_itr;
    itr->current_item_name = dir_entry.path().filename().string();
    itr->current_item_type = [dir_entry](){
        if (dir_entry.is_regular_file())
            return FW64_MEDIA_ITEM_FILE;
        if (dir_entry.is_directory())
            return FW64_MEDIA_ITEM_DIRECTORY;
        return FW64_MEDIA_ITEM_NONE;
    }();
    ++itr->dir_itr;

    return 1;
}

const char* fw64_media_dir_itr_name(fw64MediaDirItr* itr) {
    return itr->current_item_name.c_str();
}

fw64MediaItemType fw64_media_dir_itr_type(fw64MediaDirItr* itr) {
    return itr->current_item_type;
}

fw64DataSource* fw64_media_open_data_source(fw64Media* media, const char* path) {
    if (media->data_source.file.is_open())
        return nullptr;

    if (!path)
        return nullptr;

    if (path[0] == '/')
        path++;

    std::filesystem::path filesystem_path = media->base_path / path;

    if (!media->data_source.open(filesystem_path))
        return nullptr;

    return &media->data_source.interface;
}

void fw64_media_close_data_source(fw64Media* media, fw64DataSource* data_source) {
    media->data_source.file.close();
}

fw64DataWriter* fw64_media_open_data_writer(fw64Media* media, const char* path) {
    if (media->data_writer.file.is_open())
        return nullptr;

    if (!path)
        return nullptr;

    if (path[0] == '/')
        path++;

    std::filesystem::path filesystem_path = media->base_path / path;

    if (!media->data_writer.open(filesystem_path))
        return nullptr;

    return &media->data_writer.interface;
}

void fw64_media_close_data_writer(fw64Media* media, fw64DataWriter* data_writer) {
    media->data_writer.file.close();
}

fw64MediaItemType fw64_media_get_path_type(fw64Media* media, const char* path) {
    if (path == nullptr)
        return FW64_MEDIA_ITEM_NONE;

    if (path[0] == '/')
        path++;

    std::filesystem::path filesystem_path = media->base_path / path;

    if (std::filesystem::is_regular_file(filesystem_path)) {
        return FW64_MEDIA_ITEM_FILE;
    }
    else if (std::filesystem::is_directory(filesystem_path))
        return FW64_MEDIA_ITEM_DIRECTORY;
    else
        return FW64_MEDIA_ITEM_NONE;
}

int fw64_media_create_directory(fw64Media* media, const char* path) {
    if (path == nullptr)
        return 0;

    if (path[0] == '/')
        path++;

    std::filesystem::path filesystem_path = media->base_path / path;

    return static_cast<int>(std::filesystem::create_directory(filesystem_path));
}

int fw64_media_remove(fw64Media* media, const char* path) {
    if (path == nullptr)
        return 0;

    if (path[0] == '/')
        path++;

    std::filesystem::path filesystem_path = media->base_path / path;

    return static_cast<int>(std::filesystem::remove(filesystem_path));
}
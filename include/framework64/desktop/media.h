#pragma once

#include "framework64/media.h"
#include "framework64/desktop/file_datasource.h"

#include <filesystem>
#include <string>

struct fw64MediaDirItr {
    std::string path;
    std::filesystem::directory_iterator dir_itr;
    std::string current_item_name;
    fw64MediaItemType current_item_type;
};

struct fw64Media {
    bool init(std::string const & media_dir);

    std::filesystem::path base_path;
    fw64MediaDirItr dir_itr;
    framework64::FileDataSource data_source;
    framework64::FileDataWriter data_writer;
};




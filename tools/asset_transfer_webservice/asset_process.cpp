#include "asset_process.hpp"

#include "file_transfer.hpp"
#include "temp_dir.hpp"

#include <iostream>

namespace framework64::service {

bool AssetProcess::staticMesh(const std::filesystem::path& asset_path, data_link::Client& client) {
    if (!std::filesystem::exists(asset_path)) {
        std::cout << "Asset does not exist: " << asset_path << std::endl;
        return false;
    }

    TemporaryDirectory temp_dir;

    if (temp_dir.path().empty()) {
        std::cout << "Failed to create temp directory: " << temp_dir.path() << std::endl;
        return false;
    }

    std::cout << "Temporary output dir: " << temp_dir.path() << std::endl;

    auto processed_path = asset_pipeline.prepareStaticMesh(asset_path, temp_dir.path(), client.getPlatformName());

    if (processed_path.empty()) {
        return false;
    }

    FileTransfer file_transfer(client);
    file_transfer.execute(processed_path);
    
    return {};
}

}
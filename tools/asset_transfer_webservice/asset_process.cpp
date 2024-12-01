#include "asset_process.hpp"

#include "file_transfer.hpp"
#include "temp_dir.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

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

    std::cout << "Temporary workspace dir: " << temp_dir.path() << std::endl;

    const auto output_dir = temp_dir.path() / "out";
    std::filesystem::create_directory(output_dir);

    const auto assets_dir = asset_path.parent_path();
    const auto manifest_file_path = temp_dir.path() / "assets.json";
    std::cout << "Temporary asset manifest path: " << manifest_file_path << std::endl;

    // create a temporary manifest file for this mesh that will be fed to the asset pipeline
    {
        json assets_json;
        assets_json["meshes"] = json::array();
        assets_json["meshes"].push_back({{"src", asset_path.filename().string()}});

        std::ofstream assets_json_file(manifest_file_path);

        if (!assets_json_file) {
            std::cout << "Failed to create temporary asset manifest: " << manifest_file_path << std::endl;
            return false;
        }

        assets_json_file << assets_json << std::endl;
    }

    try {
        if (!asset_pipeline.run(client.getPlatformName(), manifest_file_path, assets_dir, output_dir)) {
            std::cout << "Asset pipeline processing failed." << std::endl;
            return false;
        }

        //determine the file path of the processed mesh that will be sent via datalink
        auto processed_path = output_dir/ asset_path.filename();
        processed_path.replace_extension(".mesh");

        if (!std::filesystem::exists(processed_path)) {
            std::cout << "Asset pipeline ran sucessfully but expected asset was not found: " << processed_path << std::endl;
            return false;
        }

        FileTransfer file_transfer(client);
        file_transfer.execute(processed_path);
    } catch (std::runtime_error err) {
        std::cout << "Error processing static mesh: " << err.what() << std::endl;
        return false;
    }

    return true;
}

}
#pragma once

#include <string>
#include <filesystem>

namespace framework64::service {

class AssetPipeline {
public:
    bool init(const std::string& framework64_dir);
    bool init(const std::string& framework64_dir, const std::string& node_exe_path);

    std::filesystem::path prepareStaticMesh(const std::filesystem::path& gltf_file, const std::filesystem::path& output_dir, const std::string& platform);

private:
    std::string pipeline_script_path;
    std::string node_path;
};

}

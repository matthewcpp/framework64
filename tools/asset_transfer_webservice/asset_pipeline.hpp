#pragma once

#include <string>
#include <filesystem>

namespace framework64::service {

class AssetPipeline {
public:
    bool init(const std::string& framework64_dir);
    bool init(const std::string& framework64_dir, const std::string& node_exe_path);

    bool run(const std::string& platform, const std::filesystem::path& manifest_file, const std::filesystem::path& assets_dir, const std::filesystem::path& output_dir);

private:
    std::string pipeline_script_path;
    std::string node_path;
};

}

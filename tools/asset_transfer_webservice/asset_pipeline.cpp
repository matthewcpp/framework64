#include "asset_pipeline.hpp"

#include "process.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <vector>

namespace framework64::service {

static bool testNodePath(const std::string& node_path);
static std::string findNode();

bool AssetPipeline::init(const std::string& framework64_dir) {
    return init(framework64_dir, {});
}

bool AssetPipeline::init(const std::string& framework64_dir, const std::string& node_exe_path) {
    if (node_exe_path.empty()) {
        std::cout << "Asset Pipeline: attempting to locate NodeJS" << std::endl;
        node_path = findNode();
    }
    else if (testNodePath(node_exe_path)) {
        node_path = node_exe_path;
    }

    if (node_path.empty()) {
        std::cout << "Error: Unable to locate NodeJS" << std::endl;
        return false;
    }

    std::cout << "Asset Pipeline: Using NodeJS: " << node_path << std::endl;

    auto pipeline_script_file = std::filesystem::path(framework64_dir) / "scripts" / "RunPipeline.js";
    
    if (!std::filesystem::is_regular_file(pipeline_script_file)) {
        std::cout << "Asset Pipeline: Unable to locate framework64 pipeline script: " << pipeline_script_file << std::endl;
        return false;
    }
    
    pipeline_script_path = pipeline_script_file.string();
    return true;
}

bool AssetPipeline::run(const std::string& platform, const std::filesystem::path& manifest_file, const std::filesystem::path& assets_dir, const std::filesystem::path& output_dir) {

    std::vector<std::string> command_args;
    command_args.push_back(node_path);
    command_args.push_back(pipeline_script_path);
    command_args.push_back(platform);
    command_args.push_back(manifest_file.string());
    command_args.push_back(assets_dir.string());
    command_args.push_back(output_dir.string());

    TinyProcessLib::Process pipeline_process(command_args, "", 
        [](const char *bytes, size_t n){
            std::string stdout_str(bytes, n);
            std::cout << stdout_str;
        }, 
        [](const char *bytes, size_t n){
            std::string stderr_str(bytes, n);
            std::cerr << stderr_str;
    });

    return pipeline_process.get_exit_status() == 0;
}

static const std::vector<std::string> node_paths = {
    "node",
    "/usr/local/bin/node"
};

bool testNodePath(const std::string& node_path) {
    const std::vector<std::string> args = { node_path, "--version" };
    TinyProcessLib::Process test_node(args, "");

    return test_node.get_exit_status() == 0;
}

std::string findNode() {
    for (const auto& node_path : node_paths) {
        if (testNodePath(node_path)) {
            return node_path;
        }
    }
    return {};
}

}
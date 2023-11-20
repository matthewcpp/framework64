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
        node_path = findNode();
    }
    else if (testNodePath(node_exe_path)) {
        node_path = node_exe_path;
    }

    if (node_path.empty()) {
        std::cout << "Unable to locate NodeJS" << std::endl;
        return 1;
    }

    auto pipeline_script_file = std::filesystem::path(framework64_dir) / "scripts" / "RunPipeline.js";
    
    if (!std::filesystem::is_regular_file(pipeline_script_file)) {
        std::cout << "Unable to locate framework64 pipeline script: " << pipeline_script_file << std::endl;
        return false;
    }
    
    pipeline_script_path = pipeline_script_file.string();
    return true;
}

static void writeStaticMeshAssetManifest(const std::string& gltf_filename, const std::string& file_path) {
    json assets_json;
    assets_json["meshes"] = json::array();
    assets_json["meshes"].push_back({{"src", gltf_filename}});

    std::ofstream assets_json_file(file_path);
    assets_json_file << std::setw(4) << assets_json << std::endl;
}

std::filesystem::path AssetPipeline::prepareStaticMesh(const std::filesystem::path& gltf_file, std::filesystem::path const & output_dir, const std::string& platform) {
    const auto assets_dir = gltf_file.parent_path();
    const auto manifest_file_path = output_dir / "assets.json";
    const auto gltf_filename = gltf_file.filename();
    writeStaticMeshAssetManifest(gltf_filename.string(), manifest_file_path.string());

    std::vector<std::string> command_args;
    command_args.push_back(node_path);
    command_args.push_back(pipeline_script_path);
    command_args.push_back(manifest_file_path.string());
    command_args.push_back(assets_dir.string());
    command_args.push_back(output_dir.string());
    command_args.push_back(platform);

    TinyProcessLib::Process pipeline_process(command_args, "", 
        [](const char *bytes, size_t n){
            std::string stdout_str(bytes, n);
            std::cout << stdout_str;
        }, 
        [](const char *bytes, size_t n){
        std::string stderr_str(bytes, n);
        std::cerr << stderr_str;
    });

    auto exit_status = pipeline_process.get_exit_status();

    if (exit_status != 0) {
        // pipeline output will display here
        return {};
    }

    auto mesh_file = output_dir / gltf_filename;
    mesh_file.replace_extension(".mesh");

    if (!std::filesystem::exists(mesh_file)) {
        std::cout << "pipeline completed successfully but mesh does not exist: " << mesh_file << std::endl;
        return {};
    }

    return mesh_file;
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
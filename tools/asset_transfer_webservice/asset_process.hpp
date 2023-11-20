#pragma once

#include "asset_pipeline.hpp"
#include "data_link/client.hpp"

//#include <uuid.h>

#include <filesystem>

namespace framework64::service {

class AssetProcess {
public:
    AssetProcess(AssetPipeline& pipeline) : asset_pipeline(pipeline) {}

    bool staticMesh(const std::filesystem::path& asset_path, data_link::Client& client);

private:
    AssetPipeline asset_pipeline;
    //uuids::uuid_random_generator uuid_generator;
};

}
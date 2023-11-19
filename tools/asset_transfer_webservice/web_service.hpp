#pragma once

#include "asset_pipeline.hpp"
#include "data_link/client.hpp"

#include <memory>

namespace framework64::service {

class WebService {
public:
    WebService(AssetPipeline& asset_pipeline, data_link::Client & client);
    ~WebService();

    void run(int port);
private:
    class Handler;
    std::unique_ptr<Handler> handler;
};

} 
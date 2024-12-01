#pragma once

#include "asset_pipeline.hpp"
#include "data_link/client.hpp"

// forward declaration of HttpServer from ix
// want to keep the includes in the implementation file as it will end up including windows.h
namespace ix {
    class HttpServer;
}

#include <memory>

namespace framework64::service {

class WebService {
public:
    WebService(AssetPipeline& asset_pipeline, data_link::Client & client);
    ~WebService();

    bool run(int port);

private:
    std::string onJsonPostMessage(const std::string& message);

private:
    ix::HttpServer* http_server = nullptr;
    AssetPipeline& asset_pipeline;
    data_link::Client& client;
};

} 
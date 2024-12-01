#include "web_service.hpp"

#include "asset_process.hpp"

#define NOMINMAX
#include <IXHttpServer.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <iostream>

namespace framework64::service {

WebService::WebService(AssetPipeline& asset_pipeline, data_link::Client & client)
    : asset_pipeline(asset_pipeline), client(client) {}

WebService::~WebService() {
    if (http_server) {
        http_server->stop();
        delete(http_server);
    }
}

bool WebService::run(int port) {
    
    http_server = new ix::HttpServer(port, "127.0.0.1");
    http_server->setOnConnectionCallback([this](ix::HttpRequestPtr request, std::shared_ptr<ix::ConnectionState>){
        if (request->method != "POST") {
            return std::make_shared<ix::HttpResponse>(405);
        }

        auto response = this->onJsonPostMessage(request->body);

        if (response.empty()) {
            return std::make_shared<ix::HttpResponse>(200);
        }
        else {
            return std::make_shared<ix::HttpResponse>(400);
        }
    }
    );

    auto res = http_server->listen();
    if (!res.first)
    {
        std::cout << "Failed to start webservice on port " << port << ": " << res.second << std::endl;
        return false;
    }

    std::cout << "Started webservice on port: " << port << std::endl;
    http_server->start();
    http_server->wait();

    return true;
}

std::string WebService::onJsonPostMessage(const std::string& message) {
    std::cout << "WebService::onJsonPostMessage" << std::endl;

    json req_json;
    try {
        req_json = json::parse(message);
    }
    catch(json::parse_error& ex) {
        std::cout << "Json parse error: " << ex.what() << std::endl;
        return ex.what();
    }

    auto const asset_type = req_json["type"].get<std::string>();
    auto const file_path = req_json["src"].get<std::string>();

    AssetProcess asset_process(asset_pipeline);
    asset_process.staticMesh(file_path, client);

    return {};
}


}
#include "web_service.hpp"

#include "asset_process.hpp"

#define _WIN32_WINNT 0x0601
#include <restinio/all.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace framework64::service {

class WebService::Handler {
public:
    Handler(AssetPipeline& asset_pipeline, data_link::Client & client) 
    : asset_pipeline(asset_pipeline), client(client) {}
public:
    auto processStaticMesh( const restinio::request_handle_t& req, restinio::router::route_params_t ) {
        json req_json;
        try {
            req_json = json::parse(req->body());
        }
        catch(json::parse_error& ex) {
            return parseError(req, ex.what());
        }

        auto const & file_path = req_json["src"].get<std::string>();

        AssetProcess asset_process(asset_pipeline);
        asset_process.staticMesh(file_path, client);

        return req->create_response(restinio::status_ok()).connection_close().done();
    }

    auto nonMatchedRequest(const restinio::request_handle_t& req) {
        return req->create_response(restinio::status_not_found()).connection_close().done();

    }

    restinio::request_handling_status_t parseError(const restinio::request_handle_t& req, const std::string& error_string) {
        json response_json = json::object();
        response_json["error"] = error_string;

        auto response = req->create_response(restinio::status_bad_request());
        response.set_body(response_json.dump());

        return response.connection_close().done();
    }

private:
    data_link::Client& client;
    AssetPipeline& asset_pipeline;
};

void WebService::run(int port) {
    std::cout << "Starting webservice on port: " << port << std::endl;
    auto router = std::make_unique<restinio::router::express_router_t<>>();
    router->http_post("/static-mesh", std::bind(&Handler::processStaticMesh, handler.get(), std::placeholders::_1, std::placeholders::_2));
    router->non_matched_request_handler(std::bind(&Handler::nonMatchedRequest, handler.get(), std::placeholders::_1));

    // Launching a server with custom traits.
    struct my_server_traits : public restinio::default_single_thread_traits_t {
        using request_handler_t = restinio::router::express_router_t<>;
    };

    restinio::run(
            restinio::on_this_thread<my_server_traits>()
                    .address("localhost")
                    .port(port)
                    .request_handler(std::move(router)));
}

WebService::WebService(AssetPipeline& asset_pipeline, data_link::Client & client) {
    handler = std::make_unique<Handler>(asset_pipeline, client);
}

WebService::~WebService() = default;


}
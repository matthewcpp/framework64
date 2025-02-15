#include "asset_process.hpp"
#include "get_paths.hpp"
#include "settings.hpp"
#include "web_service.hpp"

#include "data_link/desktop_client.hpp"
#include "data_link/n64_client.hpp"

#include <CLI/CLI.hpp>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

void client_message(const framework64::data_link::Message* message);

enum class Platform 
{
    None,
    Desktop,
    N64
};

std::unique_ptr<framework64::data_link::Client> createClient(Platform platform, const framework64::service::Settings& settings);

int main (int argc, char** argv) {
    Platform platform = Platform::None;
    framework64::service::Settings settings;

    CLI::App app("framework64 asset processing service");

    auto* n64_command = app.add_subcommand("n64", "transfer assets to N64");
    n64_command->add_option("--n64-rom-path", settings.n64_rom_path, "Path to data_link rom that will be loaded on N64");
    n64_command->callback([&platform](){ platform = Platform::N64; });

    auto* desktop_command = app.add_subcommand("desktop", "transfer assets to Desktop");
    desktop_command->add_option("--desktop-websocket-uri", settings.desktop_websocket_uri, "URI of data link server to connect to");
    desktop_command->callback([&platform](){ platform = Platform::Desktop; });

    app.add_option("--bind-port", settings.bind_port, "The port the service will listen to messages on.");
    app.add_option("--framework64-dir", settings.framework64_dir, "Path to the root framework64 directory");
    app.add_option("--node-path", settings.node_path, "Path to node executable");

    CLI11_PARSE(app, argc, argv);

    if (platform == Platform::None) {
        std::cout << "No platform specified.  Assuming Desktop" << std::endl;
        platform = Platform::Desktop;
    }

    if (settings.framework64_dir.empty()) {
        settings.framework64_dir = framework64::service::get_framework64_dir();
    }

    framework64::service::AssetPipeline asset_pipeline;
    if (!asset_pipeline.init(settings.framework64_dir)) {
        return 1;
    }

    auto client = createClient(platform, settings);

    if (!client) {
        return 1;
    }

    framework64::service::WebService webservice(asset_pipeline, *client);
    webservice.run(settings.bind_port);

    return 0;
}



static std::unique_ptr<framework64::data_link::DesktopClient> createDesktopClient(const framework64::service::Settings& settings) {
    auto desktop_client = std::make_unique<framework64::data_link::DesktopClient>();
    return desktop_client->connect(settings.desktop_websocket_uri) ? std::move(desktop_client) : nullptr;
}

static std::unique_ptr<framework64::data_link::N64Client> createN64Client(const framework64::service::Settings& settings) {
    auto n64_client = std::make_unique<framework64::data_link::N64Client>();
    return n64_client->connect(settings.n64_rom_path) ? std::move(n64_client) : nullptr;
}

std::unique_ptr<framework64::data_link::Client> createClient(Platform platform, const framework64::service::Settings& settings) {
    switch (platform) {
        case Platform::Desktop:
            return createDesktopClient(settings);

        case Platform::N64:
            return createN64Client(settings);

        case Platform::None:
            break;
    };

    return nullptr;
}

#include "data_link/desktop_client.hpp"
#include "data_link/n64_client.hpp"

#include <CLI/CLI.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

enum class Platform {None, Desktop, N64};

struct Settings {
    Platform platform;
    std::string n64_rom_path;
    std::string desktop_websocket_uri = "ws://localhost:55662";
};

void wait_for_message(framework64::data_link::Client & client);
std::unique_ptr<framework64::data_link::Client> create_client(const Settings& settings);

int main(int argc, char** argv) {
    CLI::App app("framework64 data_link example client");
    Settings settings;
    
    auto* n64_command = app.add_subcommand("n64", "connect to n64 example");
    n64_command->add_option("--rom", settings.n64_rom_path, "Path to data_link rom that will be loaded on N64");
    n64_command->callback([&settings](){ settings.platform = Platform::N64; });

    auto* desktop_command = app.add_subcommand("desktop", "connect to desktop example");
    desktop_command->add_option("--uri", settings.desktop_websocket_uri, "Optional URI of data link server to connect to");
    desktop_command->callback([&settings](){ settings.platform = Platform::Desktop; });

    CLI11_PARSE(app, argc, argv);

    auto client = create_client(settings);
    if (!client) {
        return 1;
    }

    // wait for the connection message coming from the example
    wait_for_message(*client);
    std::cout << "Type 'exit' to close the application." << std::endl;

    std::string message;
    for(;;) {
        std::cout << ">> ";
        std::getline(std::cin, message);

        if (message == "exit") {
            std::cout << "exiting now" << std::endl;
            break;
        }

        std::cout << "Send message: " << message << std::endl;
        client->sendMessage(reinterpret_cast<const uint8_t*>(message.c_str()), static_cast<uint32_t>(message.size()));
    }
}

void wait_for_message(framework64::data_link::Client & client) {
    for (;;) {
        auto messages_processed = client.processIncommingMessages([](const framework64::data_link::Message* message) {
            std::string server_message(reinterpret_cast<const char*>(message->buffer.data()), message->buffer.size());
            std::cout << " << " << server_message << std::endl;
        });

        if (messages_processed> 0) {
            return;
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
}

std::unique_ptr<framework64::data_link::Client> create_client(const Settings& settings) {

    switch (settings.platform) {
        case Platform::Desktop: {
            auto client_ptr = std::make_unique<framework64::data_link::DesktopClient>();
            if (client_ptr->connect(settings.desktop_websocket_uri)) {
                return std::move(client_ptr);
            }
            break;
        }

        case Platform::N64: {
            auto client_ptr = std::make_unique<framework64::data_link::N64Client>();
            if (client_ptr->connect(settings.n64_rom_path)) {
                return std::move(client_ptr);
            }
            break;
        }

        case Platform::None:
            break;
    }

    return nullptr;
}

#include "data_link/desktop_client.hpp"

#include <iostream>

namespace framework64::data_link {

bool DesktopClient::connect(const std::string& uri) {
    status = Status::Connecting;

    try {
        websocket_client.set_open_handler(std::bind(&DesktopClient::onConnectionSuccess, this, std::placeholders::_1));
        websocket_client.set_fail_handler(std::bind(&DesktopClient::onConnectionFail, this, std::placeholders::_1));
        websocket_client.set_message_handler(std::bind(&DesktopClient::onMessage, this, std::placeholders::_1, std::placeholders::_2));
        websocket_client.clear_access_channels(websocketpp::log::alevel::frame_header | websocketpp::log::alevel::frame_payload);
        websocket_client.init_asio();

        websocketpp::lib::error_code ec;
        WebsocketClient::connection_ptr con = websocket_client.get_connection(uri, ec);
        connection_handle = con->get_handle();

        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            status = Status::Failed;
            return false;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        websocket_client.connect(con);

    }
    catch(websocketpp::exception const & e) {
        std::cout << "Connection Failure: " << e.what() << std::endl;
        status = Status::Failed;
        return false;
    }

    client_thread = std::thread(clientThreadFunc, this);

    std::unique_lock unique_lock(mutex);
    condition_variable.wait(unique_lock, [this]() { return status != Status::Connecting;});

    return isConnected();
}

void DesktopClient::disconnect() {
    if (isConnected()) {
        auto connection = websocket_client.get_con_from_hdl(connection_handle);
        connection->close(websocketpp::close::status::going_away, "");
    }

    websocket_client.stop_perpetual();

    if (client_thread.joinable()) {
            client_thread.join();
    }

    status = Status::Disabled;
}

void DesktopClient::sendMessage(const uint8_t* data, size_t size) {
    if (status != Status::Connected) {
        return;
    }

    websocket_client.send(connection_handle, data, size, websocketpp::frame::opcode::value::BINARY);
}

void DesktopClient::enqueueIncomingMessage(Message::Type type, const uint8_t* data, size_t size) {
    auto message = message_pool.getMessage();
    message->type = type;
    message->buffer.resize(size);
    std::memcpy(message->buffer.data(), data, size);

    incoming_messages.enqueue(message);
}

size_t DesktopClient::processIncommingMessages(MessageProcessFunc processing_func) {
    incoming_messages.dequeueAll(incoming_messages_to_process);

    if (incoming_messages_to_process.empty()) {
        return 0;
    }

    for (const auto& message : incoming_messages_to_process) {
        processing_func(message.get());
    }

    message_pool.returnMessages(incoming_messages_to_process);
    size_t incoming_messages_size = incoming_messages_to_process.size();
    incoming_messages_to_process.clear();

    return incoming_messages_size;
}

bool DesktopClient::isConnected() const {
    return status == Status::Connected;
}

const char* DesktopClient::getPlatformName() const {
    return "desktop";
}

uint32_t DesktopClient::littleToTargetEndian(uint32_t val) {
    return val;
}

void DesktopClient::onMessage(websocketpp::connection_hdl, WebsocketClient::message_ptr msg) {
    std::cout << "Received: " << msg->get_payload().size() << " bytes" << std::endl;

    Message::Type message_type =  msg->get_opcode() == websocketpp::frame::opcode::text ? Message::Type::Text : Message::Type::Binary;
    enqueueIncomingMessage(message_type, reinterpret_cast<const uint8_t*>(msg->get_payload().c_str()), msg->get_payload().size());
}

void DesktopClient::onConnectionSuccess(websocketpp::connection_hdl handle) {
    auto connection = websocket_client.get_con_from_hdl(handle);
    std::cout << "Connected to: " << connection->get_host() << std::endl;

    setConnectionStatus(Status::Connected);
}

void DesktopClient::onConnectionFail(websocketpp::connection_hdl handle) {
    auto connection = websocket_client.get_con_from_hdl(handle);
    std::cout << "Failed to connect: " << connection->get_host() << std::endl;

    setConnectionStatus(Status::Failed);
}

void DesktopClient::setConnectionStatus(Status connection_status) {
    {
        std::lock_guard lock(mutex);
        status = connection_status;
    }

    condition_variable.notify_all();
}

void DesktopClient::clientThreadFunc(DesktopClient* desktop_client) {
    auto& websocket_client = desktop_client->websocket_client;
    
    try {
        websocket_client.start_perpetual();
        websocket_client.run();
    }
    catch(websocketpp::exception const & e) {
        std::cout << "Connection Failure: " << e.what() << std::endl;
    }
}



}
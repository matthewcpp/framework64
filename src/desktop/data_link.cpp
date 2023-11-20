#include "framework64/desktop/data_link.hpp"

#include <algorithm>
#include <iostream>

bool fw64DataLink::initialize(int port) {
    status = Status::Starting;

    try {
        websocket_server.set_open_handler(std::bind(&fw64DataLink::onClientConnect, this, std::placeholders::_1));
        websocket_server.set_message_handler(std::bind(&fw64DataLink::onMessage, this, std::placeholders::_1, std::placeholders::_2));
        websocket_server.set_access_channels(websocketpp::log::alevel::all);
        websocket_server.set_error_channels(websocketpp::log::elevel::all);

        websocket_server.init_asio();
        websocket_server.listen(port);
        websocket_server.start_accept();

        status = Status::Running;
    }
    catch(websocketpp::exception const & e) {
        std::cout << "Failed to start websocket server: " << e.what() << std::endl;
        status = Status::Failed;
    }
    server_thread = std::thread(websocketServerThread, this, port);

    std::cout << "started websocket datalink server on port: " << port << std::endl;

    return status == Status::Running;
}

void fw64DataLink::sendMessage(fw64DataLinkMessageType message_type, const uint8_t* data, size_t size) {
    auto opcode = message_type == FW64_DATA_LINK_MESSAGE_BINARY ? websocketpp::frame::opcode::binary : websocketpp::frame::opcode::text;
    websocket_server.send(websocket_connection_handle, data, size, opcode);
}

void fw64DataLink::update() {
    if (should_trigger_connected_callback) {
        status = Status::ClientConnected;
        should_trigger_connected_callback = false;

        if (connected_callback) {
            connected_callback(connected_callback_arg);
        }
    }

    {
        std::lock_guard lock(mutex);
        if (incomming_message_queue.empty()) {
            return;
        }

        processing_message_queue = std::move(incomming_message_queue);
    }

    while (!processing_message_queue.empty()) {
        auto message_ref = processing_message_queue.front();
        processing_message_queue.pop();

        if (message_callback) {
            message_callback(&message_ref->data_source, message_callback_arg);
        }

        incomming_message_pool.enqueue(message_ref);
    }
}

void fw64DataLink::setConnectedCallback(fw64DataLinkConnectedCallback callback, void* arg) {
    connected_callback = callback;
    connected_callback_arg = arg;
}

void fw64DataLink::setMessageCallback(fw64DataLinkMessageCallback callback, void* arg) {
    message_callback = callback;
    message_callback_arg = arg;
}

void fw64DataLink::onMessage(websocketpp::connection_hdl, WebsocketServer::message_ptr message) {
    auto const & payload = message->get_payload();
    auto message_ref = incomming_message_pool.dequeue();
    message_ref->assign(reinterpret_cast<const uint8_t *>(payload.data()), payload.size());
    {
        std::lock_guard lock(mutex);
        incomming_message_queue.push(message_ref);
    }
}

void fw64DataLink::onClientConnect(websocketpp::connection_hdl connection_handle) {
    websocket_connection_handle = connection_handle;
    auto connection = websocket_server.get_con_from_hdl(connection_handle);

    std::cout << "Sucessfull datalink connection from:" << connection->get_host() << std::endl;
    should_trigger_connected_callback = true;
}

void fw64DataLink::websocketServerThread(fw64DataLink* data_link, int port) {
    try {
        data_link->websocket_server.run();
    }
    catch(websocketpp::exception const & e) {
        std::cout << "Websocket server exception: " << e.what() << std::endl;
        data_link->status = Status::Failed;
    }
}

 // C API
 void fw64_data_link_set_connected_callback(fw64DataLink* data_link, fw64DataLinkConnectedCallback callback, void* arg) {
    data_link->setConnectedCallback(callback, arg);
}

void fw64_data_link_set_mesage_callback(fw64DataLink* data_link, fw64DataLinkMessageCallback callback, void* arg) {
    data_link->setMessageCallback(callback, arg);
}

int fw64_data_link_connected(fw64DataLink* data_link) {
    return static_cast<int>(data_link->isConnected());
}

void fw64_data_link_send_message(fw64DataLink* data_link, fw64DataLinkMessageType message_type, const void* data, int data_size) {
    data_link->sendMessage(message_type, reinterpret_cast<const uint8_t*>(data), data_size);
}

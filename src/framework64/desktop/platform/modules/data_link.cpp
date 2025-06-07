#include "modules/data_link.h"


#include <IXNetSystem.h>
#define NOMINMAX
#include <IXWebSocketServer.h>

#include <algorithm>
#include <iostream>
#include <memory>

fw64DataLink::fw64DataLink() {
    // Required on Windows
    ix::initNetSystem();
}

fw64DataLink::~fw64DataLink() {
    if (websocket_server) {
        websocket_server->stop();
        delete(websocket_server);
    }
}

bool fw64DataLink::initialize(int port) {
    status = Status::Starting;

    constexpr size_t max_connections = 10;
    websocket_server = new ix::WebSocketServer(port, "127.0.0.1", ix::SocketServer::kDefaultTcpBacklog, max_connections);
    // websocket_server = new ix::WebSocketServer(port, "127.0.0.1");

    websocket_server->setOnClientMessageCallback([this](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& websocket, const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
            this->should_trigger_connected_callback = true;
            std::cout << "Websocket Connected:" << connectionState->getId() << std::endl;
            connected_websocket = &websocket;
        } else if (msg->type == ix::WebSocketMessageType::Close) {
            std::cout << "Websocket Disconnected:" << connectionState->getId() << std::endl;
            connected_websocket = nullptr;
        } else if (msg->type == ix::WebSocketMessageType::Message) {
            this->onWebsocketMessage(msg->str);
        }
    });

    auto result = websocket_server->listen();
    if (!result.first) {
        status = Status::Failed;
        std::cout << "websocket datalink initialize failed: " << result.second << std::endl;
        return false;
    }

    websocket_server->start();
    status = Status::Running;

    std::cout << "started websocket datalink server on port: " << port << std::endl;

    return true;
}

void fw64DataLink::sendMessage(fw64DataLinkMessageType, const uint8_t* data, size_t size) {
    if (!connected_websocket) {
        return;
    }

    ix::IXWebSocketSendData send_data(reinterpret_cast<const char*>(data), size);
    // TODO: is it ok to send everything as binary?
    connected_websocket->sendBinary(send_data);
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

void fw64DataLink::onWebsocketMessage(const std::string& payload) {
    auto message_ref = incomming_message_pool.dequeue();
    message_ref->assign(reinterpret_cast<const uint8_t *>(payload.data()), payload.size());

    std::lock_guard lock(mutex);
    incomming_message_queue.push(message_ref);
}

 // C API
fw64DataLink* _fw64_data_link_init(void* arg) {
    const int* port_num = reinterpret_cast<int*>(arg);
    auto data_link = std::make_unique<fw64DataLink>();
    if (data_link->initialize(*port_num)) {
        return data_link.release();
    } else {
        return nullptr;
    }
 }

void _fw64_data_link_update(fw64DataLink* data_link) {
    data_link->update();
}

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

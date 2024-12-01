#include "data_link/desktop_client.hpp"

#include <IXNetSystem.h>

#include <iostream>

namespace framework64::data_link {

DesktopClient::DesktopClient() {
    // Required on Windows
    ix::initNetSystem();
}

bool DesktopClient::connect(const std::string& uri) {
    status = Status::Connecting;

    websocket.setUrl(uri);
    ix::OnMessageCallback cb;
    websocket.setOnMessageCallback(std::bind(&DesktopClient::processIxWebsocketMessage, this, std::placeholders::_1));

    websocket.start();

    // waits for the message callback to trigger that the connection has been opened
    std::unique_lock unique_lock(mutex);
    condition_variable.wait(unique_lock, [this]() { return status != Status::Connecting;});

    return isConnected();
}

void DesktopClient::disconnect() {
    if (isConnected()) {
        websocket.stop();
    }

    status = Status::Disabled;
}

void DesktopClient::processIxWebsocketMessage(const ix::WebSocketMessagePtr& msg) {
    if (msg->type == ix::WebSocketMessageType::Open){
        status = Status::Connected;
        condition_variable.notify_all();
    } else if (msg->type == ix::WebSocketMessageType::Close){
        status = Status::Disabled;
    } else if (msg->type == ix::WebSocketMessageType::Message) {
        if (msg->str.size() > 0) {
            auto message = message_pool.getMessage();
            auto buffer_data = reinterpret_cast<const uint8_t*>(msg->str.data());
            message->buffer.assign(buffer_data, buffer_data + msg->str.size());
            incoming_messages.enqueue(message);
        }
    } 
    else if (msg->type == ix::WebSocketMessageType::Error) {
        std::cout << "Error: "         << msg->errorInfo.reason      << std::endl;
        status = Status::Failed;
    }
}

void DesktopClient::sendMessage(const uint8_t* data, size_t size) {
    if (status != Status::Connected) {
        return;
    }

    // note: this call blocks until finished...might need to move to a different thread?
    ix::IXWebSocketSendData send_data(reinterpret_cast<const char*>(data), size);
    websocket.sendBinary(send_data);
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


}
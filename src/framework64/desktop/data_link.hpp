#pragma once

#include "framework64/data_link.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <thread>
#include <vector>


namespace framework64::datalink{

struct IncommingMessage {
    void assign(const uint8_t* data, size_t size) {
        payload.resize(size);
        std::memcpy(payload.data(), data, size);
        
        read_pos = 0;
    }

    static size_t read(fw64DataSource* data_source, void* buffer, size_t size, size_t count) {
        auto* message = reinterpret_cast<IncommingMessage*>(data_source);
        size_t data_remaining = message->payload.size() - message->read_pos;
        size_t read_count = size * count;
        size_t data_to_read = std::min(data_remaining, read_count);

        std::memcpy(buffer, message->payload.data() + message->read_pos, data_to_read);
        message->read_pos += data_to_read;

        return read_count;
    }

    static size_t size(fw64DataSource* data_source) {
        auto* message = reinterpret_cast<IncommingMessage*>(data_source);
        return message->payload.size();
    }

    fw64DataSource data_source = {&IncommingMessage::size, &IncommingMessage::read};
    std::vector<uint8_t> payload;
    size_t read_pos;
};

}

struct fw64DataLink {
public:
    typedef websocketpp::server<websocketpp::config::asio> WebsocketServer;

    bool initialize(int port);

    void update();

    void setConnectedCallback(fw64DataLinkConnectedCallback callback, void* arg);
    void setMessageCallback(fw64DataLinkMessageCallback callback, void* arg);
    void sendMessage(fw64DataLinkMessageType message_type, const uint8_t* data, size_t size);

    enum class Status {Disabled, Starting, Running, ClientConnected, Failed};
    inline bool isConnected() const { return status == Status::ClientConnected; }

private:
    void onMessage(websocketpp::connection_hdl, WebsocketServer::message_ptr message);
    void onClientConnect(websocketpp::connection_hdl connection_handle);

    static void websocketServerThread(fw64DataLink* data_link);

    WebsocketServer websocket_server;
    websocketpp::connection_hdl websocket_connection_handle;
    std::thread server_thread;

    fw64DataLinkMessageCallback message_callback = nullptr;
    void* message_callback_arg = nullptr;

    fw64DataLinkConnectedCallback connected_callback = nullptr;
    void* connected_callback_arg = nullptr;
    bool should_trigger_connected_callback = false;

    std::mutex mutex;
    Status status = Status::Disabled;

    template <typename T>
    class MessagePool {
    public:
        using MessageRef = std::shared_ptr<T>;

        MessageRef dequeue() {
            {
                std::lock_guard lock(mutex);
                if (!message_pool.empty()) {
                    auto message_ref = message_pool.back();
                    message_pool.pop_back();
                    return message_ref;
                }
            }

            return std::make_shared<T>();
        }

        void enqueue(MessageRef message_ref) {
            std::lock_guard lock(mutex);
            message_pool.push_back(message_ref);
        }

    private:
        std::mutex mutex;
        std::vector<MessageRef> message_pool;
    };

    using IncommingMessagePool = MessagePool<framework64::datalink::IncommingMessage>;
    IncommingMessagePool incomming_message_pool;
    std::queue<IncommingMessagePool::MessageRef> incomming_message_queue;
    std::queue<IncommingMessagePool::MessageRef> processing_message_queue;
};

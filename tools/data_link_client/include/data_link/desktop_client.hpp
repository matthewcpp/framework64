#pragma once

#include "data_link/client.hpp"
#include "data_link/message_pool.hpp"
#include "data_link/message_queue.hpp"

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <condition_variable>
#include <string>
#include <thread>

typedef websocketpp::client<websocketpp::config::asio_client> WebsocketClient;

namespace framework64::data_link {

class DesktopClient : public Client{
public:
    DesktopClient() = default;
    virtual ~DesktopClient() { disconnect(); }
    bool connect(const std::string& uri);

    virtual bool isConnected() const override;
    virtual void disconnect() override;

    virtual uint32_t littleToTargetEndian(uint32_t val) override;

    virtual void sendMessage(const uint8_t* data, size_t size) override;
    virtual size_t processIncommingMessages(MessageProcessFunc processing_func) override;
    virtual const char* getPlatformName() const override;

private:
    void onMessage(websocketpp::connection_hdl, WebsocketClient::message_ptr msg);
    void onConnectionSuccess(websocketpp::connection_hdl connection_handle);
    void onConnectionFail(websocketpp::connection_hdl connection_handle);

    enum class Status { Disabled, Connecting, Connected, Failed };

    void setConnectionStatus(Status connection_status);

    void enqueueIncomingMessage(Message::Type type, const uint8_t* data, size_t size);

    static void clientThreadFunc(DesktopClient* desktop_client);

private:
    WebsocketClient websocket_client;
    std::thread client_thread;
    websocketpp::connection_hdl connection_handle;

    std::mutex mutex;
    std::condition_variable condition_variable;
    Status status = Status::Disabled;

    MessagePool message_pool;
    MessageQueue incoming_messages;
    MessageQueue::QueueType incoming_messages_to_process;
};

}
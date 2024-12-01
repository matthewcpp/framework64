#pragma once

#include "data_link/client.hpp"
#include "data_link/message_pool.hpp"
#include "data_link/message_queue.hpp"

#include <IXWebSocket.h>

#include <condition_variable>
#include <string>
#include <thread>
#include <unordered_set>


namespace framework64::data_link {

class DesktopClient : public Client{
public:
    DesktopClient();
    virtual ~DesktopClient() { disconnect(); }
    bool connect(const std::string& uri);

    virtual bool isConnected() const override;
    virtual void disconnect() override;

    virtual uint32_t littleToTargetEndian(uint32_t val) override;

    virtual void sendMessage(const uint8_t* data, size_t size) override;
    virtual size_t processIncommingMessages(MessageProcessFunc processing_func) override;
    virtual const char* getPlatformName() const override;

private:
    enum class Status { Disabled, Connecting, Connected, Failed };

    void enqueueIncomingMessage(Message::Type type, const uint8_t* data, size_t size);
    void processIxWebsocketMessage(const ix::WebSocketMessagePtr& msg);

    ix::WebSocket websocket;

private:
    std::mutex mutex;
    std::condition_variable condition_variable;

    Status status = Status::Disabled;

    MessagePool message_pool;
    MessageQueue incoming_messages;
    MessageQueue::QueueType incoming_messages_to_process;
};

}
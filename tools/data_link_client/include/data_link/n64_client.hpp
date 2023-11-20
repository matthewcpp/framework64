#pragma once

#include "data_link/client.hpp"
#include "data_link/message_pool.hpp"
#include "data_link/message_queue.hpp"

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace framework64::data_link {

class N64Client : public Client {
public:
    N64Client();
    virtual ~N64Client() override;

    bool connect(const std::string& rom_path);
    virtual void disconnect() override;
    virtual bool isConnected() const override;

    virtual uint32_t littleToTargetEndian(uint32_t val) override;
    virtual void sendMessage(const uint8_t* data, size_t size) override;
    virtual size_t processIncommingMessages(MessageProcessFunc processing_func) override;
    virtual const char* getPlatformName() const override;

private:
    enum class Status {Disabled, Connecting, Connected, ShuttingDown, Failed};

    void sendOutgoingMessages();
    void processIncomingMessages();
    void processHeartbeat(uint32_t size, const uint8_t* buffer);

    void enqueueIncomingMessage(Message::Type type, const uint8_t* data, size_t size);

    void signalStatus(Status new_status);
    static void deviceThread(N64Client* client, const std::string rom_path);

    MessagePool message_pool;
    MessageQueue incoming_messages, outgoing_messages;
    MessageQueue::QueueType incoming_messages_to_process, outgoing_messages_to_process;

    Status status = Status::Disabled;

    std::thread device_thread;
    std::mutex mutex;
    std::condition_variable condition_variable;
};

}
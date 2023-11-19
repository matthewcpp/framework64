#pragma once

#include "message.hpp"

#include <cstdint>
#include <functional>

namespace framework64::data_link  {

using MessageProcessFunc = std::function<void(const Message*)>;

class Client {
public:
    virtual ~Client() {}
    virtual bool isConnected() const = 0;
    virtual void disconnect() = 0;
    virtual uint32_t littleToTargetEndian(uint32_t val) = 0;
    virtual void sendMessage(const uint8_t* data, size_t size) = 0;

    virtual size_t processIncommingMessages(MessageProcessFunc processing_func) = 0;
    virtual const char* getPlatformName() const = 0;
};

}
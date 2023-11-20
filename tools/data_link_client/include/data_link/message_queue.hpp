#pragma once

#include "message.hpp"

#include <algorithm>
#include <deque>
#include <iterator>
#include <mutex>

namespace framework64::data_link {

class MessageQueue {
public:
    using QueueType = std::deque<MessageRef>;

    void enqueue(MessageRef& message) {
        std::lock_guard lock(mutex);

        messages.emplace_back(message);
    }

    void dequeueAll(QueueType& queue) {
        std::lock_guard lock(mutex);

        std::copy(messages.begin(), messages.end(), std::back_inserter(queue));
        messages.clear();
    }
private:
    QueueType messages;
    std::mutex mutex;
};

}

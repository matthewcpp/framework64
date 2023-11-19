#pragma once

#include "message.hpp"

#include <memory>
#include <mutex>

namespace framework64::data_link {

class MessagePool {
public:
    MessageRef getMessage() {
        {
            std::lock_guard lock(mutex);

            if (!pool.empty()) {
                MessageRef ref = pool.back();
                pool.pop_back();
                return ref;
            }
        }

        return std::make_unique<Message>();
    }

    template <class VectorLike>
    void returnMessages(VectorLike vector_like) {
        std::lock_guard lock(mutex);
        pool.insert(pool.end(), vector_like.begin(), vector_like.end());
    }
private:
    std::vector<MessageRef> pool;
    std::mutex mutex;
};

}

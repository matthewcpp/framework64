#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace framework64::data_link {

struct Message {
    enum class Type : uint32_t{
        Binary, Text
    };

    Type type;
    std::vector<uint8_t> buffer;
};

using MessageRef = std::shared_ptr<Message>;

}

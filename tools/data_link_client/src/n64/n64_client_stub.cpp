#include "data_link/n64_client.hpp"

#include "byteswap.hpp"

namespace framework64::data_link {

N64Client::N64Client() {}
N64Client::~N64Client() {}

bool N64Client::connect(const std::string& rom_path) { return false; }
void N64Client::disconnect() {}
bool N64Client::isConnected() const { return false; }

uint32_t N64Client::littleToTargetEndian(uint32_t val) { return bswap_32(val); }
void N64Client::sendMessage(const uint8_t* data, size_t size) {}
size_t N64Client::processIncommingMessages(MessageProcessFunc processing_func) { return 0; }
const char* N64Client::getPlatformName() const { return "n64"; }
}
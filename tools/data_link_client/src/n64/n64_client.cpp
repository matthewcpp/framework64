#include "data_link/n64_client.hpp"

#include "byteswap.hpp"

#include "UNFLoader/device.h"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <filesystem>


namespace framework64::data_link {

N64Client::N64Client() {
    device_initialize();
}

N64Client::~N64Client() {
    disconnect();
}

bool N64Client::connect(const std::string& rom_path) {
    status = Status::Connecting;

    device_thread = std::thread(deviceThread, this, rom_path);

    std::unique_lock unique_lock(mutex);
    condition_variable.wait(unique_lock, [this](){ return this->status != Status::Connecting; });

    return isConnected();
}

void N64Client::signalStatus(Status new_status) {
    {
        std::lock_guard lock(mutex);
        status = new_status;
    }

    condition_variable.notify_all();
}

void N64Client::deviceThread(N64Client* client, const std::string rom_path) {
    if (!std::filesystem::is_regular_file(rom_path)) {
        std::cout << "Rom file does not exist: " << rom_path << std::endl;
        client->signalStatus(Status::Failed);
        return;
    }

    DeviceError error = device_find();
    if (error != DEVICEERR_OK) {
        std::cout << "Error finding development cart: " << error << std::endl;
        client->signalStatus(Status::Failed);
        return;
    }

    error = device_open();

    if (error != DEVICEERR_OK) {
        std::cout << "Error connecting to development cart: " << error << std::endl;
        client->signalStatus(Status::Failed);
        return;
    }

    FILE* rom_file = std::fopen(rom_path.c_str(), "rb");
    if (!rom_file) {
        std::cout << "Could not open rom file for reading: "<< rom_path << std::endl;
        device_close();
        client->signalStatus(Status::Failed);
        return;
    }

    size_t file_size = static_cast<size_t>(std::filesystem::file_size(rom_path));

    std::cout << "Uploading rom to development cart..." << std::endl;

    error = device_sendrom(rom_file, static_cast<uint32_t>(file_size));

    if (error) {
        std::cout << "Error uploading rom to development card: " << error << std::endl;
        device_close();
        client->signalStatus(Status::Failed);
        return;
    }

    std::cout << "Rom upload complete" << std::endl;
    fclose(rom_file);

    // this seems necessary to let the device send its protocol message
    // which comes from an initial heartbeat message sent from the n64
    std::this_thread::sleep_for(std::chrono::seconds(1));
    client->processIncomingMessages();

    client->signalStatus(Status::Connected);

    while (client->status != Status::ShuttingDown) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        client->processIncomingMessages();
        client->sendOutgoingMessages();
    }

    // flush any messages left to send
    client->processIncomingMessages();
    client->sendOutgoingMessages();

    std::cout << "N64: shutting down connection" << std::endl;

    device_close();
}

bool N64Client::isConnected() const {
    return status == Status::Connected;
}

const char* N64Client::getPlatformName() const {
    return "n64";
}

void N64Client::disconnect() {
    status = Status::ShuttingDown;
    if (device_thread.joinable()) {
        device_thread.join();
    }

    status = Status::Disabled;
}

void N64Client::sendOutgoingMessages() {
    outgoing_messages.dequeueAll(outgoing_messages_to_process);

    if (outgoing_messages_to_process.empty()) {
        return;
    }

    for (const auto& message : outgoing_messages_to_process) {
        DeviceError error = device_senddata(DATATYPE_RAWBINARY, const_cast<byte*>(message->buffer.data()), static_cast<uint32_t>(message->buffer.size()));

        if (error != DEVICEERR_OK) {
            std::cout << "N64: Error sending message to device: " << error << std::endl;
        }
    }

    message_pool.returnMessages(outgoing_messages_to_process);
    outgoing_messages_to_process.clear();
}

size_t N64Client::processIncommingMessages(MessageProcessFunc processing_func) {
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

void N64Client::sendMessage(const uint8_t* data, size_t size) {
    auto message = message_pool.getMessage();
    message->type = Message::Type::Binary;
    message->buffer.resize(size);
    std::memcpy(message->buffer.data(), data, size);

    outgoing_messages.enqueue(message);
}

void N64Client::processHeartbeat(uint32_t size, const uint8_t* buffer) {
    // Max supported protocol versions
    constexpr ProtocolVer USBPROTOCOL_VERSION = PROTOCOL_VERSION2;

    uint32_t header;

    if (size < 4) {
        std::cout << "Error: Malformed heartbeat received" << std::endl;
        return;
    }

    // Read the heartbeat header
    header = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | (buffer[0]);
    header = swap_endian(header);

    auto protocol_version = static_cast<ProtocolVer>((header&0xFFFF0000)>>16);
    std::cout << "N64: Setting device protocol version to " << protocol_version << std::endl;
    device_setprotocol(protocol_version);

    // Ensure we support this protocol version
    if (device_getprotocol() > USBPROTOCOL_VERSION) {
        std::cout << "N64: USB protocol " << device_getprotocol() << " unsupported. Your UNFLoader is probably out of date." << std::endl;
    }
}

void N64Client::processIncomingMessages() {
    byte* outbuff = NULL;
    uint32_t dataheader = 0;

    DeviceError error = device_receivedata(&dataheader, &outbuff);

    while (error == DEVICEERR_OK && outbuff) {
        uint32_t size = dataheader & 0xFFFFFF;
        USBDataType command = (USBDataType)((dataheader >> 24) & 0xFF);

        switch (command)
        {
            case DATATYPE_HEARTBEAT:
                processHeartbeat(size, outbuff); 
                break;

            case DATATYPE_TEXT:
                std::cout << "N64: Enqueue text message from device: " << size << " bytes" << std::endl;
                enqueueIncomingMessage(Message::Type::Text, outbuff, size);
                break;

            case DATATYPE_RAWBINARY:
                std::cout << "N64: Enqueue binary message from device: " << size << " bytes" << std::endl;
                enqueueIncomingMessage(Message::Type::Binary, outbuff, size);
                break;

            default:
                printf("N64: Unknown data type '%x'.\n", (uint32_t)command);
        }

        error = device_receivedata(&dataheader, &outbuff);
    }
}

void N64Client::enqueueIncomingMessage(Message::Type type, const uint8_t* data, size_t size) {
    auto message = message_pool.getMessage();
    message->type = type;
    message->buffer.resize(size);
    std::memcpy(message->buffer.data(), data, size);

    incoming_messages.enqueue(message);
}

uint32_t N64Client::littleToTargetEndian(uint32_t val) {
    return bswap_32(val);
}

}


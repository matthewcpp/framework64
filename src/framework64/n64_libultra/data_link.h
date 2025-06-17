#pragma once

#include "framework64/data_link.h"

#include "framework64/data_io.h"

typedef struct {
    fw64DataSource interface; 
    uint32_t size;
    uint32_t data_remaining;
} fw64N64UsbMessageStream;

void fw64_n64_usb_message_stream_init(fw64N64UsbMessageStream* stream);
void fw64_n64_usb_message_stream_reset(fw64N64UsbMessageStream* stream, uint32_t size);

size_t fw64_n64_usb_message_stream_size(fw64DataSource* stream);
size_t fw64_n64_usb_message_stream_read(fw64DataSource* stream, void* buffer, size_t size, size_t count);

#define FW64_N64_DATALINK_MAX_MESSAGE_SIZE 128
#define FW64_N64_DATALINK_MAX_MESSAGE_COUNT 5

typedef struct {
    char* buffer;
    int size;
    int type;
} fw64N64UsbMessage;

typedef struct {
    char message_data[FW64_N64_DATALINK_MAX_MESSAGE_SIZE * FW64_N64_DATALINK_MAX_MESSAGE_COUNT];
    fw64N64UsbMessage pool[FW64_N64_DATALINK_MAX_MESSAGE_COUNT];
    int size;
} fw64N64UsbOutgoingMessageQueue;

void fw64_n64_usb_outoging_message_queue_init(fw64N64UsbOutgoingMessageQueue* queue);
int fw64_n64_usb_outgoing_message_queue_enqueue(fw64N64UsbOutgoingMessageQueue* queue, fw64DataLinkMessageType type, int size, char* data);
void fw64_n64_usb_outgoing_message_queue_send(fw64N64UsbOutgoingMessageQueue* queue);

struct fw64DataLink {
    int connected;
    fw64N64UsbMessageStream message_stream;
    fw64N64UsbOutgoingMessageQueue outgoing;

    fw64DataLinkMessageCallback message_callback;
    void* message_callback_arg;

    fw64DataLinkConnectedCallback connected_callback;
    void* connected_callback_arg;
    int should_trigger_connection_callback;
};

int fw64_n64_data_link_init(fw64DataLink* data_link);
void fw64_n64_data_link_start(fw64DataLink* data_link);

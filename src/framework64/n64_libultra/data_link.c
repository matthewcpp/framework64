#include "framework64/n64_libultra/data_link.h"

#include "usb/usb.h"

#include <ultra64.h> 

#include <string.h>

int fw64_n64_data_link_init(fw64DataLink* data_link) {
    data_link->message_callback = NULL;
    data_link->message_callback_arg = NULL;

    data_link->connected_callback = NULL;
    data_link->connected_callback_arg = NULL;

    fw64_n64_usb_outoging_message_queue_init(&data_link->outgoing);
    fw64_n64_usb_message_stream_init(&data_link->message_stream);

    data_link->connected = usb_initialize();

    return data_link->connected;
}

void fw64_n64_data_link_start(fw64DataLink* data_link) {
    if (data_link->connected && data_link->connected_callback ) {
        data_link->connected_callback(data_link->connected_callback_arg);
    }
}

void fw64_data_link_set_connected_callback(fw64DataLink* data_link, fw64DataLinkConnectedCallback callback, void* arg) {
    data_link->connected_callback = callback;
    data_link->connected_callback_arg = arg;
}

void fw64_data_link_set_mesage_callback(fw64DataLink* data_link, fw64DataLinkMessageCallback callback, void* arg) {
    data_link->message_callback = callback;
    data_link->message_callback_arg = arg;
}

int fw64_data_link_connected(fw64DataLink* data_link) {
    return data_link->connected;
}

void fw64_data_link_send_message(fw64DataLink* data_link, fw64DataLinkMessageType message_type, const void* data, int data_size) {
    fw64_n64_usb_outgoing_message_queue_enqueue(&data_link->outgoing, message_type, data_size, (char*)data);
}

void fw64_n64_data_link_update(fw64DataLink* data_link) {
    // read and process any data that has arrived over the wire
    uint32_t incomming_message = usb_poll();

    
    if (incomming_message != 0) {
        // uint32_t type = USBHEADER_GETTYPE(incomming_message);
        uint32_t size = USBHEADER_GETSIZE(incomming_message);

        fw64_n64_usb_message_stream_reset(&data_link->message_stream, size);

        if (data_link->message_callback) {
            data_link->message_callback(&data_link->message_stream.interface, data_link->message_callback_arg);
        }
        
        if (data_link->message_stream.data_remaining > 0) {
            usb_purge();
        }
    }

    // send any pending messages
    fw64_n64_usb_outgoing_message_queue_send(&data_link->outgoing);
}

// Outgoing Messages

void fw64_n64_usb_outoging_message_queue_init(fw64N64UsbOutgoingMessageQueue* queue) {
    for (int i = 0; i < FW64_N64_DATALINK_MAX_MESSAGE_COUNT; i++) {
        queue->pool[i].buffer = &queue->message_data[i * FW64_N64_DATALINK_MAX_MESSAGE_SIZE];
    }

    queue->size = 0;
}

int fw64_n64_usb_outgoing_message_queue_enqueue(fw64N64UsbOutgoingMessageQueue* queue, fw64DataLinkMessageType type, int size, char* data) {
    if (queue->size >= FW64_N64_DATALINK_MAX_MESSAGE_COUNT || size > FW64_N64_DATALINK_MAX_MESSAGE_SIZE)
        return 0;

    fw64N64UsbMessage* message = &queue->pool[queue->size++];

    message->type = type == FW64_DATA_LINK_MESSAGE_BINARY ? DATATYPE_RAWBINARY : DATATYPE_TEXT;
    message->size = size;
    memcpy(message->buffer, data, size);

    return queue->size;
}

void fw64_n64_usb_outgoing_message_queue_send(fw64N64UsbOutgoingMessageQueue* queue) {
    for (int i = 0; i < queue->size; i++) {
        fw64N64UsbMessage* message = &queue->pool[i];

        usb_write(message->type, message->buffer, message->size);
    }

    queue->size = 0;
}

// USB Message Stream

void fw64_n64_usb_message_stream_init(fw64N64UsbMessageStream* stream) {
    stream->interface.read = fw64_n64_usb_message_stream_read;
    stream->interface.size = fw64_n64_usb_message_stream_size;
}

void fw64_n64_usb_message_stream_reset(fw64N64UsbMessageStream* stream, uint32_t size) {
    stream->size = size;
    stream->data_remaining = size;
}

size_t fw64_n64_usb_message_stream_size(fw64DataSource* stream) {
    fw64N64UsbMessageStream* message_stream = (fw64N64UsbMessageStream*)stream;

    return message_stream->size;
}

size_t fw64_n64_usb_message_stream_read(fw64DataSource* stream, void* buffer, size_t size, size_t count) {
    fw64N64UsbMessageStream* message_stream = (fw64N64UsbMessageStream*)stream;

    size_t bytes_to_read = size * count;
    bytes_to_read = bytes_to_read < message_stream->data_remaining ? bytes_to_read : message_stream->data_remaining;

    usb_read(buffer, (int)bytes_to_read);

    message_stream->data_remaining -= bytes_to_read;

    return bytes_to_read;
}
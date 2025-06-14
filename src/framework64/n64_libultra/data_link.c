#include "framework64/n64_libultra/data_link.h"
#include "framework64/n64_libultra/log.h"

#include "usb/usb.h"

#include <ultra64.h> 

#include <stdio.h>
#include <string.h>

fw64DataLink _dl;

static void fw64_n64_data_link_update(void* data_link, void* arg);
static void fw64_n64_data_link_log(const char* message, va_list args);

fw64DataLink* fw64_data_link_init(fw64Engine* engine) {
    fw64DataLink* data_link = &_dl;

    _fw64_modules_register_static(engine->modules, FW64_DATALINK_MODULE_ID, data_link, fw64_n64_data_link_update, NULL);

    data_link->message_callback = NULL;
    data_link->message_callback_arg = NULL;

    data_link->connected_callback = NULL;
    data_link->connected_callback_arg = NULL;

    fw64_n64_usb_outoging_message_queue_init(&data_link->outgoing);
    fw64_n64_usb_message_stream_init(&data_link->message_stream);

    data_link->connected = usb_initialize();
    data_link->should_trigger_connection_callback = 1;

    #ifndef NDEBUG
    fw64_n64libultra_log_set_func(fw64_n64_data_link_log);
    #endif

    return data_link;
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

void fw64_n64_data_link_update(void* dl, void* arg) {
    fw64DataLink* data_link = (fw64DataLink*)dl;
    (void)arg;

    if (data_link->should_trigger_connection_callback && data_link->connected) {
        if (data_link->connected_callback) {
            data_link->connected_callback(data_link->connected_callback_arg);
        }

        data_link->should_trigger_connection_callback = 0;
    }

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

#ifndef NDEBUG
extern int _Printf(void *(*copyfunc)(void *, const char *, size_t), void*, const char*, va_list);
static void* printf_handler(void *buf, const char *str, size_t len)
{
    return ((char *) memcpy(buf, str, len) + len);
}

#define FW64_DATA_LINK_MESSAGE_SIZE 128

void fw64_n64_data_link_log(const char* message, va_list args) {
    char message_buffer[FW64_DATA_LINK_MESSAGE_SIZE];
    int message_length = _Printf(printf_handler, message_buffer, message, args);
    fw64_data_link_send_message(&_dl, Fw64_DATA_LINK_MESSAGE_TEXT, &message_buffer[0], message_length);
}

#endif